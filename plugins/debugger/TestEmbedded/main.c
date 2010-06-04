#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Linked List
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
			linked_list* tmp=list;
			if(destructor)
				destructor(list->data);
			free(list);
			list=list->next;
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
			printf("linked_list_push_front: list & list2 are not empty\n");
			linked_list_last_element(list2)->next=list;
			return list2;
		}
		else if(!linked_list_is_empty(list))
		{
			//List2 is empty, let's return list
			printf("linked_list_push_front: list2 is empty\n");
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

	void linked_list_print(const linked_list* list, linked_list_data_show show)
	{
		printf("List(l=%i): %s", linked_list_length(list), list?"":"\n");
		while(list)
		{
			if(show)
				printf("(%s) %s", show(list->data), list->next?"--> ":"\n");
			else
				printf("(%p) %s", list->data, list->next?"--> ":"\n");
			list=list->next;
		}
	}
	
// Debug
	// Variable
	typedef struct
	{
		char* name;
		int size;
		void* data;
	}variable;
	
	variable* variable_create(const char* name, int size, void* data)
	{
		variable* var = (variable*)malloc(sizeof(variable));
		
		var->name=strdup(name);
		var->size=size;
		var->data=data;
		
		return var;
	}
	
	variable_free(variable* var)
	{
		if(var)
		{
			free(var->name);
			free(var->data);
			free(var);
		}
	}

	//Frames
	linked_list* frame_create()
	{
		linked_list* f=linked_list_create();
		return f;
	}
	
	void frame_free(linked_list* frame)
	{
		linked_list* tmp=frame;
		while(tmp)
		{
			variable_free((variable*)tmp->data);
			tmp=linked_list_next_element(tmp);
		}
	}
	
	//Debug
	linked_list* frames;
	
	void DbgInit()
	{
		frames=linked_list_create();
	}
	
	void DbgNewFrame()
	{
		// Create a new frame and add it as an element on the frames' list
		linked_list_element_push_front(frames, frame_create());
	}
	
	char* show_variable(void* data)
	{
		variable* var=(variable*)data;
		
		if(var==NULL)
			return NULL;
		
		int size=1+strlen(var->name)+3+sizeof(void*)+1;
		char* ret=(char*)malloc(size);
		
		int pos=snprintf(ret, size, "'%s'='%X'", var->name, var->data);

		return ret;
	}
	
	void _DbgWatchVariable(const char* name, int size, void* data)
	{
		variable* var=variable_create(name, size, data);
		
		linked_list_first_element(frames)->data=linked_list_element_push_front((linked_list*)(linked_list_first_element(frames)->data), var);
		
		linked_list_print((linked_list*)(linked_list_first_element(frames)->data), NULL);
	}

	
	void printCurrentFrameVariables()
	{
		//
		linked_list_print((linked_list*)frames->data, show_variable);
	}

// Us
char* show_string(void* data)
{
	return (char*) data;
}

int main(int argc, char** argv)
{
	char a=-53, b=127, c=200;
	
	DbgInit();
	
	_DbgWatchVariable("a", sizeof(a), &a);
	_DbgWatchVariable("b", sizeof(b), &b);
	_DbgWatchVariable("c", sizeof(c), &c);
	
	printCurrentFrameVariables();
	
	
	
	/*linked_list* test=linked_list_create();
	
	linked_list_print(test, show_string);
	test=linked_list_element_push_back(test, "poulpe");
	linked_list_print(test, show_string);
	test=linked_list_element_push_back(test, "poulpe2");
	linked_list_print(test, show_string);
	test=linked_list_element_push_back(test, "poulpe3");
	linked_list_print(test, show_string);
	test=linked_list_push_front(test, linked_list_create());
	linked_list_print(test, show_string);
	
	linked_list_free(test, NULL);*/
	
	return 0;
}