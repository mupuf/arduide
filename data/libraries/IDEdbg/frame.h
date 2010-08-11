#ifndef FRAME_H
#define FRAME_H
	#include "variable.h"
	#include "linked_list.h"
	
	typedef struct
	{
		int line;
		char* name;
		linked_list* vars;
	} frame;
	
	frame* frame_create(int l, const char* name);
	
	void frame_free(frame* frame);	
	void frame_free(void* f);
	
	linked_list* frame_get_variables(frame* frame);
	
	void frame_add_variable(frame* frame, variable* var);
	
	void generateFrameTrace(frame* frame);
#endif
