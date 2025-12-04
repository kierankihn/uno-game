/**
 * @file main.cpp
 *
 * @author Yuzhe Guo
 * @date 2025.12.01
 */
#include <argparse/argparse.hpp>

#include "UnoServer.h"
int main(int argc, char *argv[])
{
    argparse::ArgumentParser parser("Uno Server", "0.1.0");

    parser.add_argument("-p", "--port").help("server port").default_value(static_cast<uint16_t>(10001)).scan<'i', uint16_t>();

    try {
        parser.parse_args(argc, argv);
    }
    catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return 1;
    }

    try {
        UNO::SERVER::UnoServer uno_server(parser.get<uint16_t>("--port"));
        uno_server.run();
    }
    catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}