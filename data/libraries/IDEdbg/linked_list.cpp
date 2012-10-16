/*
  linked_list.cpp

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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "linked_list.h"

linked_list* linked_list_create()
{
	linked_list* list=(linked_list*)malloc(sizeof(linked_list));
	
	list->next=NULL;
	list->data=NULL;
	
	return list;
}

void linked_list_free(linked_list* list, linked_list_data_freer destructor)
{
	while(list)
	{
		linked_list* next=list->next;
		if(destructor)
			destructor(list->data);
		free(list);
		list=next;
	}
}

int linked_list_is_empty(const linked_list* list)
{
	return list==NULL || list->data==NULL;
}

int linked_list_length(const linked_list* list)
{
	int i=0;
	while(list)
	{
		if(list->data!=NULL)
			i++;
		
		list=list->next;
	}
	return i;
}

linked_list* linked_list_first_element(linked_list* list)
{
	return list;
}

linked_list* linked_list_last_element(linked_list* list)
{
	if(list)
	{
		linked_list* tmp=list;
		while(tmp->next)
			tmp=tmp->next;
		return tmp;
	}
	else
		return list;
}

linked_list* linked_list_push_front(linked_list* list, linked_list* list2)
{
	if(!linked_list_is_empty(list) && !linked_list_is_empty(list2))
	{
		linked_list_last_element(list2)->next=list;
		return list2;
	}
	else if(!linked_list_is_empty(list))
	{
		//List2 is empty, let's return list
		return list;
	}
	else
	{
		/*if(linked_list_is_empty(list2))
			printf("linked_list_push_front: both list1 and list2 are empty.\n");*/
		
		//List1 is empty
		if(list)
			linked_list_free(list, NULL);
		
		//let's return list2
		return list2;
	}
}

linked_list* linked_list_push_back(linked_list* list, linked_list* list2)
{
	if(!linked_list_is_empty(list) && !linked_list_is_empty(list2))
	{
		linked_list_last_element(list)->next=list2;
		return list;
	}
	else if(!linked_list_is_empty(list))
		return list;
	else
	{
		//List1 is empty
		if(list)
			linked_list_free(list, NULL);
		
		//let's return list2
		return list2;
	}
}

linked_list* linked_list_element_push_front(linked_list* list, void* data)
{
	linked_list* element=linked_list_create();
	element->data=data;
	return linked_list_push_front(list, element);
}

linked_list* linked_list_element_push_back(linked_list* list, void* data)
{
	linked_list* element=linked_list_create();
	element->data=data;
	return linked_list_push_back(list, element);
}

linked_list* linked_list_next_element(linked_list* list)
{
	if(!linked_list_is_empty(list))
		return list->next;
	else
		return NULL;
}

linked_list* linked_list_remove_front(linked_list* list, linked_list_data_freer destructor)
{
	if(list==NULL)
		return NULL;
	
	linked_list* tmp=list;
	list=list->next;
	
	if(destructor)
		destructor(tmp->data);
	free(tmp);

	return list;
}

void linked_list_print(const linked_list* list, linked_list_data_show show)
{
	printf("List(l=%i): %s", linked_list_length(list), list?"":"\n");
	while(list)
	{
		if(show)
		{
			char* data=show(list->data);
			printf("(%s) %s", data, list->next?"--> ":"\n");
			free(data);
		}
		else
			printf("(%p) %s", list->data, list->next?"--> ":"\n");
		list=list->next;
	}
}

char* linked_list_show_string(void* data)
{
	return (char*) data;
}
