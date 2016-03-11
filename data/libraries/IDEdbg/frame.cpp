/*
  frame.cpp

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

#include <string.h>

#include "frame.h"
#include "IDEdbgPrivate.h"

frame* frame_create(int l, const char* name)
{
	frame* f=(frame*)malloc(sizeof(frame));
	f->line=l;
	f->name=strdup(name);
	f->vars=NULL;
	return f;
}

void frame_free(frame* frame)
{
	if(frame==NULL)
		return;

	// Free the variables
	linked_list_free(frame->vars, variable_free);

	// Free the name
	free(frame->name);

	// Free the struct
	free(frame);
}

void frame_free(void* f)
{
	frame_free((frame*)f);
}

linked_list* frame_get_variables(frame* frame)
{
	if(frame!=NULL)
		return frame->vars;
	else
		return NULL;
}

void frame_add_variable(frame* frame, variable* var)
{
	if(frame==NULL || var==NULL)
		return;

	frame->vars=linked_list_element_push_front(frame->vars, var);
}

void generateFrameTrace(frame* frame)
{
	if(frame==NULL)
		return;

	// Frame header's size
	DbgPrintf("<frame l=\"%i\" id=\"%s\">", frame->line, frame->name);

	// Variables' size
	linked_list* f_vars=frame_get_variables(frame);
	int varCount=linked_list_length(f_vars);

	for(int i=0; i<varCount; ++i)
	{
		print_variable((variable*)f_vars->data);
		f_vars=f_vars->next;
	}

	Serial.print("</frame>");
}
