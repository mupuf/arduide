#ifndef FRAME_H
#define FRAME_H
	#include <stdlib.h>
	#include <string.h>
	
	#include "variable.h"
	#include "linked_list.h"
	
	typedef struct
	{
		char* name;
		linked_list* vars;
	} frame;
	
	frame* frame_create(const char* name)
	{
		frame* f=(frame*)malloc(sizeof(frame));
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
		
		printf("Add variable=%p\n", var);
		
		frame->vars=linked_list_element_push_front(frame->vars, var);
	}
	
	char* generateFrameTrace(frame* frame)
	{
		if(frame==NULL)
			return NULL;
		
		// the string that will be returned
		char* ret;
		
		// Frame header's size
		int sizeRet=strlen("frame '")+strlen(frame->name)+3; // "frame 'name': "
		
		// Variables' size
		linked_list* f_vars=frame_get_variables(frame);
		int varCount=linked_list_length(f_vars);
		char** variables=(char**)malloc(varCount*sizeof(char*));
		
		// Get all the text for the variables
		int i;
		for(i=0; i<varCount; ++i)
		{
			variables[i]=show_variable(f_vars->data);
			sizeRet+=strlen(variables[i]);
			
			f_vars=f_vars->next;
		}
		
		// Add the final \0 char
		++sizeRet;
		
		// Allocate the final string
		ret=(char*)malloc(sizeRet*sizeof(char));
		
		// add the header
		int pos=snprintf(ret, sizeRet, "frame '%s': ", frame->name);
		
		// Create a single string that will contain all this
		for(i=0; i<varCount; ++i)
		{
			pos+=snprintf(ret+pos, sizeRet-pos, "%s", variables[i]);
			free(variables[i]);
		}
		
		// Free the each variable string
		free(variables);
		
		return ret;
	}

#endif
