#include "UnoClient.h"

#include <windows.h>

int main()
{
    UNO::CLIENT::UnoClient client;
    client.run();
    return 0;
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    main();
    return 0;
}