#pragma once

#include <cstdint>
#include <filesystem>
#include <format>
#include <mutex>
#include <source_location>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

enum class LogLevel : uint8_t
{
    Trace = 0,
    Debug,
    Info,
    Warn,
    Error,
    Critical,
    Off
};

struct LoggerConfig
{
    LogLevel minimumLevel = LogLevel::Info;
    bool consoleEnabled = true;
    bool fileEnabled = true;
    bool includeSourceLocation = true;
    std::filesystem::path logDirectory = "logs";
    std::string fileStem = "life3d";
};

struct LogRecord
{
    LogLevel level = LogLevel::Info;
    std::string category;
    std::string message;
    std::source_location location = std::source_location::current();
};

class Logger
{
public:
    static Logger& Instance();

    void Initialize(LoggerConfig config);
    void Shutdown();
    void Flush();

    void SetMinimumLevel(LogLevel level);
    LogLevel MinimumLevel() const;
    bool ShouldLog(LogLevel level) const;

    void Write(const LogRecord& record);

    template <typename... Args>
    void LogAt(
        LogLevel level,
        std::source_location location,
        std::string_view category,
        std::format_string<Args...> format,
        Args&&... args)
    {
        if (!ShouldLog(level))
        {
            return;
        }

        Write(LogRecord{
            level,
            std::string(category),
            std::format(format, std::forward<Args>(args)...),
            location});
    }

private:
    Logger() = default;

    std::string FormatRecord(const LogRecord& record) const;
    void OpenFileSink();

    mutable std::mutex m_mutex;
    LoggerConfig m_config{};
    std::filesystem::path m_logPath;
    bool m_initialized = false;
};

const char* LogLevelName(LogLevel level);
LogLevel LogLevelFromString(std::string_view value, LogLevel fallback = LogLevel::Info);

#define LIFE3D_LOG_TRACE(...) \
    Logger::Instance().LogAt(LogLevel::Trace, std::source_location::current(), __VA_ARGS__)

#define LIFE3D_LOG_DEBUG(...) \
    Logger::Instance().LogAt(LogLevel::Debug, std::source_location::current(), __VA_ARGS__)

#define LIFE3D_LOG_INFO(...) \
    Logger::Instance().LogAt(LogLevel::Info, std::source_location::current(), __VA_ARGS__)

#define LIFE3D_LOG_WARN(...) \
    Logger::Instance().LogAt(LogLevel::Warn, std::source_location::current(), __VA_ARGS__)

#define LIFE3D_LOG_ERROR(...) \
    Logger::Instance().LogAt(LogLevel::Error, std::source_location::current(), __VA_ARGS__)

#define LIFE3D_LOG_CRITICAL(...) \
    Logger::Instance().LogAt(LogLevel::Critical, std::source_location::current(), __VA_ARGS__)
