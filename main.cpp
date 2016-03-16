/*
  main.cpp

  This file is part of arduide, The Qt-based IDE for the open-source Arduino electronics prototyping platform.

  Copyright (C) 2010-2016 
  Authors : Denis Martinez
	    Martin Peres 

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
