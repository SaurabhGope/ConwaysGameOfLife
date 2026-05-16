#include "core/Application.hpp"
#include "core/Logger.hpp"

#include <cstdlib>
#include <optional>
#include <string>

namespace
{
std::optional<std::string> GetEnvironmentValue(const char* name)
{
#if defined(_WIN32)
    char* value = nullptr;
    size_t length = 0;
    if (_dupenv_s(&value, &length, name) != 0 || value == nullptr)
    {
        return std::nullopt;
    }

    std::string result(value);
    std::free(value);
    return result;
#else
    if (const char* value = std::getenv(name))
    {
        return std::string(value);
    }
    return std::nullopt;
#endif
}

LoggerConfig CreateLoggerConfig()
{
    LoggerConfig config{};
#if defined(NDEBUG)
    config.minimumLevel = LogLevel::Info;
#else
    config.minimumLevel = LogLevel::Debug;
#endif

    if (const auto level = GetEnvironmentValue("LIFE3D_LOG_LEVEL"))
    {
        config.minimumLevel = LogLevelFromString(*level, config.minimumLevel);
    }

    if (const auto directory = GetEnvironmentValue("LIFE3D_LOG_DIR"))
    {
        config.logDirectory = *directory;
    }

    return config;
}
}

int main()
{
    Logger::Instance().Initialize(CreateLoggerConfig());
    LIFE3D_LOG_INFO("app", "Life3D process starting.");

    AppConfig config{};
    Application app(config);
    const int result = app.Run();

    LIFE3D_LOG_INFO("app", "Life3D process exiting with code {}.", result);
    Logger::Instance().Shutdown();
    return result;
}
