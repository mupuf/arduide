/*
  linked_list.h

  This file is part of arduide, The Qt-based IDE for the open-source Arduino electronics prototyping platform.

  Copyright (C) 2010-2012 
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

#ifndef LINKED_LIST_H
#define LINKED_LIST_H

	typedef struct _linked_list
	{
		struct _linked_list* next;
		void* data;
	}linked_list;
	
	typedef void (*linked_list_data_freer)(void*);
	typedef char* (*linked_list_data_show)(void*);

	linked_list* linked_list_create();	
	void linked_list_free(linked_list* list, linked_list_data_freer destructor);
	
	int linked_list_is_empty(const linked_list* list);
	int linked_list_length(const linked_list* list);
	linked_list* linked_list_next_element(linked_list* list);
	
	linked_list* linked_list_first_element(linked_list* list);
	linked_list* linked_list_last_element(linked_list* list);
	
	linked_list* linked_list_push_front(linked_list* list, linked_list* list2);
	linked_list* linked_list_push_back(linked_list* list, linked_list* list2);
	linked_list* linked_list_element_push_front(linked_list* list, void* data);
	linked_list* linked_list_element_push_back(linked_list* list, void* data);
	
	linked_list* linked_list_remove_front(linked_list* list, linked_list_data_freer destructor);
	
	void linked_list_print(const linked_list* list, linked_list_data_show show);
	
	char* linked_list_show_string(void* data);
#endif
