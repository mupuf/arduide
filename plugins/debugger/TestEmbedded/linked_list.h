#ifndef LINKED_LIST_H
#define LINKED_LIST_H
	#include <stdlib.h>
	#include <string.h>

	typedef struct _linked_list
	{
		struct _linked_list* next;
		void* data;
	}linked_list;
	
	typedef void (*linked_list_data_freer)(void*);
	typedef char* (*linked_list_data_show)(void*);

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
			if(linked_list_is_empty(list2))
				printf("linked_list_push_front: both list1 and list2 are empty.\n");
			
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
#endif
