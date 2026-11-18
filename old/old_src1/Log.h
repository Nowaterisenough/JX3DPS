#ifndef JX3DPS_LOG_H
#define JX3DPS_LOG_H

#include <spdlog/spdlog.h>

namespace JX3DPS {

class Log
{
public:
    template <typename T>
    static void Info(const T &msg)
    {
        Logger()->log(spdlog::level::info, msg);
    }

    template <typename... Args>
    static void Info(spdlog::format_string_t<Args...> fmt, Args &&...args)
    {
        Logger()->log(spdlog::level::info, fmt, std::forward<Args>(args)...);
    }

    template <typename T>
    static void Warn(const T &msg)
    {
        Logger()->log(spdlog::level::warn, msg);
    }

    template <typename... Args>
    static void Warn(spdlog::format_string_t<Args...> fmt, Args &&...args)
    {
        Logger()->log(spdlog::level::warn, fmt, std::forward<Args>(args)...);
    }

    template <typename T>
    static void Error(const T &msg)
    {
        Logger()->log(spdlog::level::err, msg);
    }

    template <typename... Args>
    static void Error(spdlog::format_string_t<Args...> fmt, Args &&...args)
    {
        Logger()->log(spdlog::level::err, fmt, std::forward<Args>(args)...);
    }

    template <typename T>
    static void Debug(const T &msg)
    {
        Logger()->log(spdlog::level::debug, msg);
    }

    template <typename... Args>
    static void Debug(spdlog::format_string_t<Args...> fmt, Args &&...args)
    {
        Logger()->log(spdlog::level::debug, fmt, std::forward<Args>(args)...);
    }

    template <typename T>
    static void Trace(const T &msg)
    {
        Logger()->log(spdlog::level::trace, msg);
    }

    template <typename... Args>
    static void Trace(spdlog::format_string_t<Args...> fmt, Args &&...args)
    {
        Logger()->log(spdlog::level::trace, fmt, std::forward<Args>(args)...);
    }

    template <typename T>
    static void Critical(const T &msg)
    {
        Logger()->log(spdlog::level::critical, msg);
    }

    template <typename... Args>
    static void Critical(spdlog::format_string_t<Args...> fmt, Args &&...args)
    {
        Logger()->log(spdlog::level::critical, fmt, std::forward<Args>(args)...);
    }

    // void SetLogLevel(spdlog::level::level_enum level) { _logger->set_level(level); }

    inline static std::string log_file_name = "log";

private:
    Log();

    static std::shared_ptr<spdlog::logger> Logger();

    inline static std::shared_ptr<spdlog::logger> _logger = nullptr;
};

} // namespace JX3DPS

#endif // JX3DPS_LOG_H