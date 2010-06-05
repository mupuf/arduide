#ifndef FRAME_H
#define FRAME_H
	#include <stdlib.h>
	#include <string.h>
	
	#include "variable.h"
	#include "linked_list.h"
	
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
	
	char* generateFrameTrace(linked_list* frame)
	{
		int varCount=linked_list_length(frame);
		char** variables=(char**)malloc(varCount*sizeof(char*));
		
		int sizeRet=0;
		char* ret;
		
		// Get all the text for the variables
		int i;
		for(i=0; i<varCount; ++i)
		{
			variables[i]=show_variable(frame->data);
			sizeRet+=strlen(variables[i]);
			
			frame=frame->next;
		}
		
		// Add the needed space for the ; separator
		sizeRet+=varCount-1;
		
		// Add the final \0 char
		++sizeRet;
		
		// Allocate the final string
		ret=(char*)malloc(sizeRet*sizeof(char));
		
		// Create a single string that will contain all this
		int pos=0;
		for(i=0; i<varCount; ++i)
		{
			pos+=snprintf(ret+pos, sizeRet-pos, "%s%s", variables[i], (i<varCount-1)?";":"");
			free(variables[i]);
		}
		
		// Free the each variable string
		free(variables);
		
		return ret;
	}

#endif
