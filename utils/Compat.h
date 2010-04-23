/**
 * \file Compat.h
 * \author Denis Martinez
 */

#ifndef COMPAT_H
#define COMPAT_H

#if defined(Q_OS_WIN32) || defined(Q_OS_WIN64)
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace Compat
{

#if defined(Q_OS_WIN32) || defined(Q_OS_WIN64)
void sleep_ms(int ms)
{
	Sleep(ms);
}
#else
void sleep_ms(int ms)
{
	usleep(ms * 1000);
}
#endif

}

#endif // COMPAT_H
