#ifndef IDEDBG_H
#define IDEDBG_H
	#include "linked_list.h"
	#include "variable.h"
	#include "frame.h"
	
	// Private
	char* IDEdbg_getFrames();
	
	// Public
	void DbgInit(int baud_rate);
	
	void _DbgNewFrame(int l, const char* name);
	#define DbgNewFrame(X) (_DbgNewFrame(__LINE__, X))
	
	void DbgCloseFrame();

    void DbgFree();
	
	void _DbgWatchVariable(int l, const char* name, variable_type type, int size, void* data);
	void _DbgWatchVariable(int l, const char* name, int* data);
	void _DbgWatchVariable(int l, const char* name, unsigned int* data);
	void _DbgWatchVariable(int l, const char* name, char* data);
	void _DbgWatchVariable(int l, const char* name, unsigned char* data);
	void _DbgWatchVariable(int l, const char* name, float* data);
	void _DbgWatchVariable(int l, const char* name, double* data);
	void _DbgWatchVariable(int l, const char* name, const char** data);
	void _DbgWatchVariable(int l, const char* name, char** data);
	void _DbgWatchVariable(int l, const char* name, void* data);
	#define DbgWatchVariable(X) (_DbgWatchVariable(__LINE__, #X, &X))

    void DbgSendChar(char c);
    void DbgSendString(const char* s);
	void DbgSendTrace(const char* format, ...);
	
	void _DbgSendState(const char* filename, int line);
	#define DbgSendState() _DbgSendState(__FILE__, __LINE__);
	
	// Useless
	void IDEdbg_printCurrentFrameVariables();
#endif
