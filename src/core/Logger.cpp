#include "core/Logger.hpp"

#include <algorithm>
#include <chrono>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>
#include <utility>

namespace
{
std::ofstream& FileStream()
{
    static std::ofstream stream;
    return stream;
}

std::string Lowercase(std::string_view value)
{
    std::string result(value);
    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return result;
}

std::string TimestampForLine()
{
    const auto now = std::chrono::system_clock::now();
    const auto time = std::chrono::system_clock::to_time_t(now);
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    std::tm localTime{};
#if defined(_WIN32)
    localtime_s(&localTime, &time);
#else
    localtime_r(&time, &localTime);
#endif

    std::ostringstream stream;
    stream << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S")
        << '.' << std::setw(3) << std::setfill('0') << ms.count();
    return stream.str();
}

std::string TimestampForFile()
{
    const auto now = std::chrono::system_clock::now();
    const auto time = std::chrono::system_clock::to_time_t(now);

    std::tm localTime{};
#if defined(_WIN32)
    localtime_s(&localTime, &time);
#else
    localtime_r(&time, &localTime);
#endif

    std::ostringstream stream;
    stream << std::put_time(&localTime, "%Y%m%d-%H%M%S");
    return stream.str();
}

std::filesystem::path ResolveLogDirectory(std::filesystem::path path)
{
    if (path.is_absolute())
    {
        return path;
    }

    return std::filesystem::current_path() / path;
}
}

Logger& Logger::Instance()
{
    static Logger logger;
    return logger;
}

void Logger::Initialize(LoggerConfig config)
{
    std::lock_guard lock(m_mutex);
    m_config = std::move(config);
    m_initialized = true;
    OpenFileSink();

    const LogRecord record{
        LogLevel::Info,
        "logger",
        std::format("Logger initialized. min_level={} file={}", LogLevelName(m_config.minimumLevel), m_logPath.string()),
        std::source_location::current()};
    const std::string line = FormatRecord(record);
    if (m_config.consoleEnabled)
    {
        std::clog << line << '\n';
    }
    if (m_config.fileEnabled && FileStream().is_open())
    {
        FileStream() << line << '\n';
        FileStream().flush();
    }
}

void Logger::Shutdown()
{
    std::lock_guard lock(m_mutex);
    if (!m_initialized)
    {
        return;
    }

    const std::string line = FormatRecord(LogRecord{
        LogLevel::Info,
        "logger",
        "Logger shutdown.",
        std::source_location::current()});
    if (m_config.consoleEnabled)
    {
        std::clog << line << '\n';
    }
    if (m_config.fileEnabled && FileStream().is_open())
    {
        FileStream() << line << '\n';
        FileStream().flush();
        FileStream().close();
    }
    m_initialized = false;
}

void Logger::Flush()
{
    std::lock_guard lock(m_mutex);
    std::clog.flush();
    if (FileStream().is_open())
    {
        FileStream().flush();
    }
}

void Logger::SetMinimumLevel(LogLevel level)
{
    std::lock_guard lock(m_mutex);
    m_config.minimumLevel = level;
}

LogLevel Logger::MinimumLevel() const
{
    std::lock_guard lock(m_mutex);
    return m_config.minimumLevel;
}

bool Logger::ShouldLog(LogLevel level) const
{
    std::lock_guard lock(m_mutex);
    return m_initialized &&
        m_config.minimumLevel != LogLevel::Off &&
        static_cast<uint8_t>(level) >= static_cast<uint8_t>(m_config.minimumLevel);
}

void Logger::Write(const LogRecord& record)
{
    std::lock_guard lock(m_mutex);
    if (!m_initialized ||
        m_config.minimumLevel == LogLevel::Off ||
        static_cast<uint8_t>(record.level) < static_cast<uint8_t>(m_config.minimumLevel))
    {
        return;
    }

    const std::string line = FormatRecord(record);
    if (m_config.consoleEnabled)
    {
        std::clog << line << '\n';
    }
    if (m_config.fileEnabled && FileStream().is_open())
    {
        FileStream() << line << '\n';
    }
}

std::string Logger::FormatRecord(const LogRecord& record) const
{
    std::ostringstream stream;
    stream << '[' << TimestampForLine() << ']'
        << '[' << LogLevelName(record.level) << ']'
        << '[' << record.category << ']'
        << "[tid:" << std::this_thread::get_id() << "] "
        << record.message;

    if (m_config.includeSourceLocation)
    {
        stream << " (" << record.location.file_name() << ':' << record.location.line() << ')';
    }

    return stream.str();
}

void Logger::OpenFileSink()
{
    if (FileStream().is_open())
    {
        FileStream().close();
    }

    if (!m_config.fileEnabled)
    {
        return;
    }

    const auto directory = ResolveLogDirectory(m_config.logDirectory);
    std::error_code error;
    std::filesystem::create_directories(directory, error);
    if (error)
    {
        m_config.fileEnabled = false;
        return;
    }

    m_logPath = directory / std::format("{}-{}.log", m_config.fileStem, TimestampForFile());
    FileStream().open(m_logPath, std::ios::out | std::ios::app);
    if (!FileStream().is_open())
    {
        m_config.fileEnabled = false;
    }
}

const char* LogLevelName(LogLevel level)
{
    switch (level)
    {
    case LogLevel::Trace:
        return "TRACE";
    case LogLevel::Debug:
        return "DEBUG";
    case LogLevel::Info:
        return "INFO";
    case LogLevel::Warn:
        return "WARN";
    case LogLevel::Error:
        return "ERROR";
    case LogLevel::Critical:
        return "CRITICAL";
    case LogLevel::Off:
        return "OFF";
    }

    return "UNKNOWN";
}

LogLevel LogLevelFromString(std::string_view value, LogLevel fallback)
{
    const std::string normalized = Lowercase(value);
    if (normalized == "trace")
    {
        return LogLevel::Trace;
    }
    if (normalized == "debug")
    {
        return LogLevel::Debug;
    }
    if (normalized == "info")
    {
        return LogLevel::Info;
    }
    if (normalized == "warn" || normalized == "warning")
    {
        return LogLevel::Warn;
    }
    if (normalized == "error")
    {
        return LogLevel::Error;
    }
    if (normalized == "critical" || normalized == "fatal")
    {
        return LogLevel::Critical;
    }
    if (normalized == "off" || normalized == "none")
    {
        return LogLevel::Off;
    }

    return fallback;
}
