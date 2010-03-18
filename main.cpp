/**
 * \file main.cpp
 * \author Denis Martinez
 */

#include "IDEApplication.h"

int main(int argc, char** argv)
{
    IDEApplication *app = new IDEApplication(argc, argv);
    app->initialize();
    return app->exec();
}

#if defined(Q_OS_WIN32) || defined(Q_OS_WIN64)
#include <windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    return main(__argc, __argv);
}
#endif
