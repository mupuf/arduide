#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "linked_list.h"
#include "variable.h"
#include "frame.h"
#include "IDEdbg.h"
#include "IDEdbgPrivate.h"
#include "IDEdbgConstants.h"

// Contains the list of the frames
linked_list* frames = NULL;

static unsigned char readValue()
{
	unsigned char value;
	do
	{
		value = Serial.read();
	} while(value == 255);
	return value;
}

static char* readString()
{
	char* ret;
	unsigned char size = readValue();
	
	DbgSendTrace("Read string: size=%i", size);

	ret = (char*)malloc(size*sizeof(char));
	
	for(int i=0; i<size; i++)
	{
		ret[i] = readValue();
		DbgSendTrace("Read string: read ret[%i]=%c", i, ret[i]);
	}
	
	return ret;
}

static void miniShell()
{
	static bool inMiniShell = false;
	
	// Stop if the user didn't ask for the minishell
	if (inMiniShell || Serial.available()==0)
	{
		//DbgPrintf("<No commands>");
		return;
	}

	// If the command was not for a shell, exit
	unsigned char cmd = readValue();
	if(cmd != SHELL_REQUESTED)
	{
		DbgPrintf("<error>Unknown command %i</error>", cmd);
		return;
	}
	else
		DbgPrintf("<ret v=\"OK\" />");
	
	inMiniShell = true;

	// Enter the miniShell
	while(true)
	{
		// Get the command
		unsigned char cmd = readValue();
		if(cmd == DIGITAL_READ)
		{
			unsigned char pin = readValue();
			int pin_val = digitalRead(pin);
			const char* ret = (pin_val==HIGH?"HIGH":"LOW");
			DbgPrintf("<ret v=\"%s\"/>", ret);
		}
		else if(cmd == DIGITAL_WRITE)
		{
			unsigned char pin = readValue();
			unsigned char value = readValue();
			digitalWrite(pin, value==1?HIGH:LOW);
			DbgPrintf("<ret v=\"OK\"/>");
		}
		else if(cmd == ANALOG_READ)
		{
			unsigned char pin = readValue();
			int pin_val = analogRead(pin);
			DbgPrintf("<ret v=\"%i\"/>", pin_val);
		}
		else if(cmd == ANALOG_WRITE)
		{
			unsigned char pin = readValue();
			unsigned char value = readValue();
			analogWrite(pin, value);
			DbgPrintf("<ret v=\"OK\"/>");
		}
		else if(cmd == PIN_MODE)
		{
			unsigned char pin = readValue();
			unsigned char mode = readValue();
			pinMode(pin, mode==1?OUTPUT:INPUT);
			DbgPrintf("<ret v=\"OK\"/>");
		}
		else if(cmd == VAR_WRITE)
		{
			DbgSendTrace("Will read string");
			char* frameName=readString();
			char* varName=readString();
			char* value=readString();
			DbgSendTrace("frame=%s, varName=%s, value=%s", frameName, varName, value);
			free(frameName);
			free(varName);
			free(value);
			
			DbgPrintf("<ret v=\"OK\"/>");
		}
		else if(cmd == EXIT_SHELL)
		{
			DbgPrintf("<ret v=\"OK\"/>");
			break;
		}
		else
		{
			DbgPrintf("<error>Unknown command %i</error>", cmd);

			//Empty the input buffer
			while (Serial.available()>0)
				Serial.read();

			// Leave the minishell
			break;
		}
	}
	
	inMiniShell = false;
}

// Public
void DbgInit(int baud_rate)
{
	DbgFree();
	Serial.begin(baud_rate);
}

void _DbgNewFrame(int l, const char* name)
{
	// Create a new frame and add it as an element on the frames' list
	frames=linked_list_element_push_front(frames, frame_create(l, name));
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
		Serial.write(c);
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
				Serial.write(format[i]);
				++i;
			}
		}
		else
		{
            if (escapeFormat)
                DbgSendChar(format[i]);
            else
                Serial.write(format[i]);
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

	miniShell();
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

	miniShell();
}

// Useless
void IDEdbg_printCurrentFrameVariables()
{
	// Just print the list
	linked_list_print((linked_list*)frames->data, show_variable);
}
