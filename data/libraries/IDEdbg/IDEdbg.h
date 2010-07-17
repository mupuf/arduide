#ifndef IDEDBG_H
#define IDEDBG_H
	#include "linked_list.h"
	#include "variable.h"
	#include "frame.h"
	
	// Private
	char* IDEdbg_getFrames();
	
	// Public
	void DbgInit(int baud_rate);
	
	void DbgNewFrame(const char* name);
	
	void DbgCloseFrame();
	
	void _DbgWatchVariable(const char* name, variable_type type, int size, void* data);
	void _DbgWatchVariable(const char* name, int* data);
	void _DbgWatchVariable(const char* name, unsigned int* data);
	void _DbgWatchVariable(const char* name, char* data);
	void _DbgWatchVariable(const char* name, unsigned char* data);
	void _DbgWatchVariable(const char* name, float* data);
	void _DbgWatchVariable(const char* name, double* data);
	void _DbgWatchVariable(const char* name, const char** data);
	void _DbgWatchVariable(const char* name, char** data);
	void _DbgWatchVariable(const char* name, void* data);
	#define DbgWatchVariable(X) (_DbgWatchVariable(#X, &X))
	
	void DbgSendTrace(const char* format, ...);
	
	void _DbgSendState(const char* filename, int line);
	#define DbgSendState() _DbgSendState(__FILE__, __LINE__);
	
	// Useless
	void IDEdbg_printCurrentFrameVariables();
#endif
