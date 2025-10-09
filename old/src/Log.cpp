#include "Log.h"

#include <iomanip>

#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

using namespace JX3DPS;

std::shared_ptr<spdlog::logger> Log::Logger()
{
    static Log                             log;
    static std::shared_ptr<spdlog::logger> logger = _logger;
    return logger;
}

Log::Log()
{
    // 获取当前时间
    auto now    = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);

    // 转换为本地时间
    std::tm tm;
#ifdef _WIN32
    localtime_s(&tm, &time_t); // Windows 系统使用 localtime_s
#else
    localtime_r(&time_t, &tm); // POSIX 系统使用 localtime_r
#endif
    // 格式化时间到字符串
    std::stringstream time_stream;
    time_stream << std::put_time(&tm, "%Y-%m-%d"); // 格式化为 Year-Month-Day
    std::string date_str = time_stream.str();

    // 日志文件名使用当前日期
    std::string log_file = "logs/" + log_file_name + "_" + date_str + ".txt";

    // 创建控制台sink
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

    // 创建文件sink
    auto file_sink =
        std::make_shared<spdlog::sinks::rotating_file_sink_mt>(log_file, 1024 * 1024 * 10, 3);

    // 创建spdlog的logger，绑定两个sink
    _logger = std::make_shared<spdlog::logger>("multi_sink", spdlog::sinks_init_list{ console_sink, file_sink });

    // 设置日志记录级别
    _logger->set_level(spdlog::level::info);

    spdlog::set_global_logger(_logger);
}