/**
 * @file main.cpp
 *
 * @author Yuzhe Guo
 * @date 2025.12.01
 */
#include "../common/Logger.h"
#include <argparse/argparse.hpp>
#include <spdlog/spdlog.h>

#include "UnoServer.h"
int main(int argc, char *argv[])
{
    UNO::COMMON::Logger::init("uno-server");
    SPDLOG_INFO("Starting uno-server");

    argparse::ArgumentParser parser("Uno Server", "0.1.0");

    parser.add_argument("-p", "--port").help("server port").default_value(static_cast<uint16_t>(10001)).scan<'i', uint16_t>();

    try {
        parser.parse_args(argc, argv);
    }
    catch (const std::exception &e) {
        SPDLOG_ERROR("Argument parsing failed: {}", e.what());
        return 1;
    }

    try {
        auto port = parser.get<uint16_t>("--port");
        SPDLOG_INFO("Launching server on port {}", port);
        UNO::SERVER::UnoServer uno_server(port);
        uno_server.run();
    }
    catch (const std::exception &e) {
        SPDLOG_ERROR("Server crashed with exception: {}", e.what());
        return 1;
    }

    return 0;
}