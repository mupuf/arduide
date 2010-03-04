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

bool setDTR(HANDLE h, bool enable)
{
    return EscapeCommFunction(h, enable ? SETDTR : CLRDTR);
}
#else
void sleep_ms(int ms)
{
	usleep(ms * 1000);
}

bool setDTR(int fd, bool enable)
{
    unsigned int result = 0;
    if (ioctl(fd, TIOCMGET, &result) == -1)
        return false;
    if (enable)
        result |= TIOCM_DTR;
    else
        result &= ~ TIOCM_DTR;
    if (ioctl(fd, TIOCMSET, &result) == -1)
        return false;
    return true;
}
#endif

}

#endif // COMPAT_H
