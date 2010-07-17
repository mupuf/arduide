#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "linked_list.h"
#include "variable.h"
#include "frame.h"

#define FRAME_END 0
#define FRAME_TRACE 1
#define FRAME_SEND_STATE 2

// Contains the list of the frames
linked_list* frames;

// Private
char* IDEdbg_getFrames()
{
	int frameCount=linked_list_length(frames);
	char** traces=(char**)malloc(frameCount*sizeof(char*));
	
	// Result string
	int sizeRet=strlen("<frames>\n");
	char* ret;
	
	// Get a string from each frame
	linked_list* tmpFrame=linked_list_first_element(frames);
	int i;
	for(i=0; tmpFrame!=NULL && i<frameCount; ++i)
	{
		traces[i]=generateFrameTrace((frame*)tmpFrame->data);
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

// Public
void DbgInit()
{
	frames=NULL;
	Serial.begin(9600); 
}

void DbgNewFrame(const char* name)
{
	// Create a new frame and add it as an element on the frames' list
	frames=linked_list_element_push_front(frames, frame_create(name));
}

void DbgCloseFrame()
{
	linked_list_remove_front(frames, frame_free);
}

void _DbgWatchVariable(const char* name, variable_type type, int size, void* data)
{
	variable* var=variable_create(name, type, size, data);
	
	frame_add_variable((frame*)linked_list_first_element(frames)->data, var);
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

void DbgSendTrace(const char* format, ...)
{
	va_list list;
	va_start(list, format);
	
	// Send the trace byte
	Serial.print(FRAME_TRACE, BYTE);
	
	// Read all the format string
	int i=0;
	while(format[i])
	{
		if(format[i]=='%')
		{
			// Get the next char
			++i;
			
			// If we are not at the end of the string
			if(format[i]==NULL)
			{
			}
			else if(format[i]=='i')
			{
				int value=va_arg(list, int);
				Serial.print(value, DEC);
				++i;
			}
			else if(format[i]=='x')
			{
				int value=va_arg(list, int);
				Serial.print("0x");
				Serial.print(value, HEX);
				++i;
			}
			else if(format[i]=='c')
			{
				int value=va_arg(list, int);
				Serial.print(value, BYTE);
				++i;
			}
			else if(format[i]=='s')
			{
				char* value=va_arg(list, char*);
				Serial.print(value);
				++i;
			}
			else //if(format[i]=='%')
			{
				Serial.print(format[i], BYTE);
				++i;
			}
		}
		else
		{
			Serial.print(format[i], BYTE);
			++i;
		}
	}
	
	// Send the end trace byte
	Serial.print(FRAME_END, BYTE);

	va_end(list);
}

void _DbgSendState(const char* filename, int line)
{
	char* data=IDEdbg_getFrames();
	
	Serial.print(FRAME_SEND_STATE, BYTE);
	printf("Send state at %s:%i\n%s", filename, line, data);
	
	// Send the end trace byte
	Serial.print(FRAME_END, BYTE);
	free(data);
}

// Useless
void IDEdbg_printCurrentFrameVariables()
{
	// Just print the list
	linked_list_print((linked_list*)frames->data, show_variable);
}
