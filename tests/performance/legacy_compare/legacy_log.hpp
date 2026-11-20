#pragma once

#include <bq_log/bq_log.h>

namespace legacy_compare {

inline bq::log &Logger()
{
    static auto logger =
        bq::log::create_log("jx3dps_legacy",
                            "log.thread_mode=sync\n"
                            "appenders_config.console.type=console\n"
                            "appenders_config.console.levels=[all]");
    return logger;
}

} // namespace legacy_compare
