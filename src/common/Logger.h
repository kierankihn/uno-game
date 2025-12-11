/**
 * @file Logger.h
 */
#pragma once

#include <string>

namespace UNO::COMMON {
    struct Logger {
        // Initialize global logging. Safe to call multiple times.
        // Default log directory aligns with CMake runtime output: <build>/bin/log
        static void init(const std::string &app_name, const std::string &log_dir = "log");
    };
}   // namespace UNO::COMMON
