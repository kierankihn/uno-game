/**
 * @file Logger.cpp
 */
#include "Logger.h"

#include <filesystem>
#include <memory>

#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

namespace fs = std::filesystem;

namespace UNO::COMMON {
    void Logger::init(const std::string &app_name, const std::string &log_dir)
    {
        fs::path dir{log_dir};
        if (!fs::exists(dir)) {
            fs::create_directories(dir);
        }

        auto logfile = (dir / (app_name + ".log")).string();

        constexpr std::size_t max_file_size = 5 * 1024 * 1024;
        constexpr std::size_t max_files     = 3;

        auto rotating_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(logfile, max_file_size, max_files);
        auto console_sink  = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

        rotating_sink->set_level(spdlog::level::debug);
        console_sink->set_level(spdlog::level::info);

        std::vector<spdlog::sink_ptr> sinks{rotating_sink, console_sink};
        auto logger = std::make_shared<spdlog::logger>(app_name, sinks.begin(), sinks.end());

        logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%s:%#] [%!] %v");

        spdlog::register_logger(logger);
        spdlog::set_default_logger(logger);
    }
}   // namespace UNO::COMMON
