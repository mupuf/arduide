/*
  IDEPluginInterface.h

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
 * \file IDEPluginInterface.h
 * \author Denis Martinez
 */

#ifndef IDEPLUGININTERFACE_H
#define IDEPLUGININTERFACE_H

#include <QtPlugin>

class IDEApplication;

class IDEPluginInterface
{
public:
    virtual ~IDEPluginInterface();
    virtual bool setup(IDEApplication *app) = 0;
    virtual const QString &name() = 0;
};

Q_DECLARE_INTERFACE(IDEPluginInterface, "org.mupuf.ArduIDE.IDEPluginInterface/1.0")

#endif // IDEPLUGININTERFACE_H
