/**
 * \file main.cpp
 * \author Denis Martinez
 */

#include "IDEApplication.h"

int main(int argc, char** argv)
{
    IDEApplication *app = new IDEApplication(argc, argv);
    return app->exec();
}

#if defined(Q_OS_WIN32) || defined(Q_OS_WIN64)
#include <windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
#define CONSOLE 0
#if CONSOLE
    if (! AllocConsole())
        return 1;

    FILE *stream;
    freopen_s(&stream, "CONIN$", "rb", stdin);
    freopen_s(&stream, "CONOUT$", "wb", stdout);
    freopen_s(&stream, "CONOUT$", "wb", stderr);
    std::ios::sync_with_stdio();
#endif

    int ec = main(__argc, __argv);

#if CONSOLE
    FreeConsole();
#endif
    return ec;
}
#endif
