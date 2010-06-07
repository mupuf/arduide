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
