#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "linked_list.h"
#include "variable.h"
#include "frame.h"
#include "IDEdbg.h"

// Contains the list of the frames
linked_list* frames = NULL;

// Public
void DbgInit(int baud_rate)
{
	DbgFree();
	Serial.begin(baud_rate);
}

void DbgNewFrame(const char* name)
{
	// Create a new frame and add it as an element on the frames' list
	frames=linked_list_element_push_front(frames, frame_create(name));
}

void DbgCloseFrame()
{
	frames=linked_list_remove_front(frames, frame_free);
}

void DbgFree()
{
	while(frames)
		DbgCloseFrame();
}

void _DbgWatchVariable(int l, const char* name, variable_type type, int size, void* data)
{
	variable* var=variable_create(l, name, type, size, data);
	
	frame_add_variable((frame*)linked_list_first_element(frames)->data, var);
}

void _DbgWatchVariable(int l, const char* name, int* data)
{
	return _DbgWatchVariable(l, name, _int, sizeof(&data), (void*)data);
}

void _DbgWatchVariable(int l, const char* name, unsigned int* data)
{
	return _DbgWatchVariable(l, name, _unsigned_int, sizeof(&data), (void*)data);
}

void _DbgWatchVariable(int l, const char* name, char* data)
{
	return _DbgWatchVariable(l, name, _char, sizeof(&data), (void*)data);
}

void _DbgWatchVariable(int l, const char* name, unsigned char* data)
{
	return _DbgWatchVariable(l, name, _unsigned_char, sizeof(&data), (void*)data);
}

void _DbgWatchVariable(int l, const char* name, float* data)
{
	return _DbgWatchVariable(l, name, _float, sizeof(&data), (void*)data);
}

void _DbgWatchVariable(int l, const char* name, double* data)
{
	return _DbgWatchVariable(l, name, _double, sizeof(&data), (void*)data);
}

void _DbgWatchVariable(int l, const char* name, const char** data)
{
	return _DbgWatchVariable(l, name, _char_pointer, sizeof(&data), (void*)data);
}

void _DbgWatchVariable(int l, const char* name, char** data)
{
	return _DbgWatchVariable(l, name, (const char**)data);
}

void _DbgWatchVariable(int l, const char* name, void* data)
{
	return _DbgWatchVariable(l, name, _void_pointer, sizeof(&data), (void*)data);
}

void DbgSendChar(char c)
{
    if(c == '<')
        Serial.print("&lt;");
    else if(c == '>')
        Serial.print("&gt;");
    else
        Serial.print(c, BYTE);
}

void DbgSendString(const char* s)
{
    while(*s)
    {
        DbgSendChar(*s);
        s++;
    }
}

static void _DbgPrintf(const char* format, bool escapeFormat, va_list list)
{
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
				DbgSendChar(value);
				++i;
			}
			else if(format[i]=='s')
			{
				char* value=va_arg(list, char*);
				DbgSendString(value);
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
            if (escapeFormat)
                DbgSendChar(format[i]);
            else
                Serial.print(format[i], BYTE);
			++i;
		}
	}
}

void DbgPrintf(const char* format, ...)
{
    va_list list;
    va_start(list, format);

    _DbgPrintf(format, false, list);

    va_end(list);
}

void DbgSendTrace(const char* format, ...)
{
    va_list list;
    va_start(list, format);

    Serial.print("<trace>");
    _DbgPrintf(format, true, list);
    Serial.print("</trace>");

    va_end(list);
}

void _DbgSendState(const char* filename, int line)
{
    DbgPrintf("<frames l=\"%i\">", line);

	int frameCount=linked_list_length(frames);
	linked_list* tmpFrame=linked_list_first_element(frames);
	
	for(int i=0; tmpFrame!=NULL && i<frameCount; ++i)
	{
		generateFrameTrace((frame*)tmpFrame->data);
		tmpFrame=tmpFrame->next;
	}

	Serial.print("</frames>");
}

// Useless
void IDEdbg_printCurrentFrameVariables()
{
	// Just print the list
	linked_list_print((linked_list*)frames->data, show_variable);
}
