#include <stdio.h>

#include "IDEdbg.h"

int main(int argc, char** argv)
{
	char a='a';
	int b=127;
	int c=-25;
	
	const char* str="Je suis mupuf";
	
	float f=1234.123456789;
	double d=123456.123456789;
	
	DbgInit();
	
	DbgNewFrame("poulpe");
	
	DbgWatchVariable(a);
	DbgWatchVariable(b);
	DbgWatchVariable(c);
	DbgWatchVariable(f);
	DbgWatchVariable(d);
	
	DbgSendState();
	
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