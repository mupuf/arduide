#ifndef IDEDBG_H
#define IDEDBG_H

	#include "linked_list.h"
	#include "variable.h"
	#include "frame.h"
	
	//Debug
	linked_list* frames;
	
	void DbgInit()
	{
		frames=NULL;
	}
	
	void DbgNewFrame()
	{
		// Create a new frame and add it as an element on the frames' list
		frames=linked_list_element_push_front(frames, frame_create());
	}
	
	void DbgCloseFrame()
	{
		// Get the current frame
		linked_list* top=linked_list_first_element(frames);
		
		// Do not free an empty frame :)
		if(linked_list_is_empty(top))
		{
			printf("No more frame to close\n");
			return;
		}
		
		// The new top frame is the next frame
		frames=top->next;
		
		// Get the variable list
		linked_list* vars=(linked_list*)top->data;
		
		// Free the frame
		frame_free(vars);
	}
	
	void _DbgWatchVariable(const char* name, variable_type type, int size, void* data)
	{
		variable* var=variable_create(name, type, size, data);
		
		linked_list_first_element(frames)->data=linked_list_element_push_front((linked_list*)(linked_list_first_element(frames)->data), var);
	}
	
	void _DbgWatchVariable(const char* name, int* data)
	{
		return _DbgWatchVariable(name, _int, sizeof(&data), (void*)data);
	}
	
	void _DbgWatchVariable(const char* name, unsigned int* data)
	{
		return _DbgWatchVariable(name, _unsigned_int, sizeof(&data), (void*)data);
	}
	
	void _DbgWatchVariable(const char* name, char* data)
	{
		return _DbgWatchVariable(name, _char, sizeof(&data), (void*)data);
	}
	
	void _DbgWatchVariable(const char* name, unsigned char* data)
	{
		return _DbgWatchVariable(name, _unsigned_char, sizeof(&data), (void*)data);
	}
	
	void _DbgWatchVariable(const char* name, float* data)
	{
		return _DbgWatchVariable(name, _float, sizeof(&data), (void*)data);
	}
	
	void _DbgWatchVariable(const char* name, double* data)
	{
		return _DbgWatchVariable(name, _double, sizeof(&data), (void*)data);
	}
	
	void _DbgWatchVariable(const char* name, const char** data)
	{
		return _DbgWatchVariable(name, _char_pointer, sizeof(&data), (void*)data);
	}
	
	void _DbgWatchVariable(const char* name, char** data)
	{
		return _DbgWatchVariable(name, (const char**)data);
	}
	
	void _DbgWatchVariable(const char* name, void* data)
	{
		return _DbgWatchVariable(name, _void_pointer, sizeof(&data), (void*)data);
	}
	
	#define DbgWatchVariable(X) (_DbgWatchVariable(#X, &X))
	
	// Private
	char* IDEdbg_generateDebuggingTraces()
	{
		int frameCount=linked_list_length(frames);
		char** traces=(char**)malloc(frameCount*sizeof(char*));
		
		// Result string
		int sizeRet=strlen("<frames>\n");
		char* ret;
		
		// Get a string from each frame
		linked_list* tmpFrame=frames;
		int i;
		for(i=0; i<frameCount; ++i)
		{
			traces[i]=generateFrameTrace((linked_list*)tmpFrame->data);
			sizeRet+=strlen(traces[i]);
			tmpFrame=tmpFrame->next;
		}
		
		// Add the needed space for the \n separator
		sizeRet+=frameCount;
		
		// Add the final </frames>
		sizeRet+=strlen("</frames>\n");
		
		// Add the final \0 char
		++sizeRet;
		
		// Allocate the final string
		ret=(char*)malloc(sizeRet*sizeof(char));
		
		// Copy the frame tag
		int pos=snprintf(ret, sizeRet, "<frames>\n");
		
		// Create a single string that will contain all this
		for(i=0; i<frameCount; ++i)
		{
			pos+=snprintf(ret+pos, sizeRet-pos, "%s\n", traces[i]);
			free(traces[i]);
		}
		
		// Copy the closing frames tag
		pos+=snprintf(ret+pos, sizeRet-pos, "<\\frames>\n");
		
		// Free the each variable string
		free(traces);
		
		return ret;
	}
	
	void IDEdbg_printCurrentFrameVariables()
	{
		// Just print the list
		linked_list_print((linked_list*)frames->data, show_variable);
	}
#endif
