
#include <cstdint>
#include <signal.h>
#include <fstream>
#include <string>
#include <sys/stat.h>

#include "utils.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include <spdlog/async.h>
#include <spdlog/cfg/env.h> // support for loading levels from the environment variable
#include <spdlog/fmt/ostr.h> // support for user defined types
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/spdlog.h>

#define DEFAULT_LOG_PATH "logs"

bool log_level_trace = false;

void set_log_level(const std::string& level)
{
    if (level == "trace")
    {
        log_level_trace = true;
        spdlog::set_level(spdlog::level::trace);
    }
    else if (level == "debug")
    {
        spdlog::set_level(spdlog::level::debug);
    }
    else if (level == "info")
    {
        spdlog::set_level(spdlog::level::info);
    }
    else if (level == "warn")
    {
        spdlog::set_level(spdlog::level::warn);
    }
    else if (level == "error")
    {
        spdlog::set_level(spdlog::level::err);
    }
    else if (level == "fatal")
    {
        spdlog::set_level(spdlog::level::critical);
    }
    else if (level == "none")
    {
        spdlog::set_level(spdlog::level::off);
    }
    else
    {
        spdlog::set_level(spdlog::level::info);
    }
}
void init_logs(const  std::string& logpath, const char* logname, const char* name,
    bool console)
{


    if (!mkpath_if_not_exist(logpath))
    {
        return;
    }


    std::string full = path_join(logpath, logname);

    auto rotating_logger =
        spdlog::rotating_logger_mt(name, full, 1048576 * 20, 20);
    //  auto new_logger = spdlog::basic_logger_mt("new_default_logger",
    //  "logs/new-default-log.txt", true);


    printf("open logger %s, %s\n", name, full.c_str());

    spdlog::init_thread_pool(8192, 1);

    auto rotating_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(full, 1048576 * 20, 20);
    std::vector<spdlog::sink_ptr> sinks;
    if (console)
    {
        auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt >();
        sinks.push_back(stdout_sink);
    }

    sinks.push_back(rotating_sink);

    auto logger = std::make_shared<spdlog::async_logger>("default", sinks.begin(), sinks.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::block);
    // spdlog::register_logger(logger); //<-- this line registers logger for spdlog::get

    spdlog::set_default_logger(logger);



    /* */
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%t] [%^%l%$] : %v");
    spdlog::set_level(spdlog::level::info);
}
