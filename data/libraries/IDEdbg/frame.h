/*
  frame.h

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

#ifndef FRAME_H
#define FRAME_H
	#include "variable.h"
	#include "linked_list.h"
	
	typedef struct
	{
		int line;
		char* name;
		linked_list* vars;
	} frame;
	
	frame* frame_create(int l, const char* name);
	
	void frame_free(frame* frame);	
	void frame_free(void* f);
	
	linked_list* frame_get_variables(frame* frame);
	
	void frame_add_variable(frame* frame, variable* var);
	
	void generateFrameTrace(frame* frame);
#endif
