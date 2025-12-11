#include "UnoClient.h"

#include "../common/Logger.h"
#include <spdlog/spdlog.h>
#include <windows.h>

int main()
{
    UNO::COMMON::Logger::init("uno-client");
    SPDLOG_INFO("Starting uno-client application");

    UNO::CLIENT::UnoClient client;
    client.run();

    SPDLOG_INFO("uno-client exited");
    return 0;
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    main();
    return 0;
}