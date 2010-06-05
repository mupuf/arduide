#include <stdio.h>

#include "IDEdbg.h"

int main(int argc, char** argv)
{
	char a=-53, b=127, c=200;
	
	DbgInit();
	
	DbgNewFrame();
	
	_DbgWatchVariable("a", sizeof(a), &a);
	_DbgWatchVariable("b", sizeof(b), &b);
	
	DbgNewFrame();
	
	_DbgWatchVariable("c", sizeof(c), &c);
	
	printCurrentFrameVariables();
	
	DbgCloseFrame();
	
	char* data=generateDebuggingTraces();
	printf("data=\"%s\"\n", data);
	free(data);
	
	DbgCloseFrame();
	
	DbgCloseFrame();
	
	
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