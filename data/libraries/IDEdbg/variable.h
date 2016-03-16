/*
  variable.h

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

#ifndef VARIABLE_H
#define VARIABLE_H

	typedef enum {_error=-1, _int=0, _unsigned_int=1, _char=2, _unsigned_char=3, _float=4, _double=5, _char_pointer=6, _void_pointer=7}variable_type;

	typedef struct
	{
		int line;
		char* name;
		variable_type type;
		int size;
		void* data;
	}variable;

	const char* variable_type_to_string(variable_type type);
	variable_type variable_type_from_string(const char* type);

	variable* variable_create(int line, const char* name, variable_type type, int size, void* data);

	void variable_set_value(variable* var, void* data, size_t size);
	void variable_set_value(variable* var, char* data);

	void variable_free(variable* var);
	void variable_free(void* var);

	char* show_variable(void* data);
	void print_variable(variable* var);
#endif
