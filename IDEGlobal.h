/**
 * \file IDEGlobal.h
 * \author Denis Martinez
 */

#ifndef IDEGLOBAL_H
#define IDEGLOBAL_H

#include <QtGlobal>

#ifdef IDE_PLUGIN
#define IDE_EXPORT Q_DECL_IMPORT
#else
#define IDE_EXPORT Q_DECL_EXPORT
#endif

#endif // IDEGLOBAL_H
