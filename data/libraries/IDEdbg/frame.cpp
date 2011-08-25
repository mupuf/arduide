#include <string.h>

#include "frame.h"
#include "IDEdbgPrivate.h"

frame* frame_create(int l, const char* name)
{
	frame* f=(frame*)malloc(sizeof(frame));
	f->line=l;
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

	frame->vars=linked_list_element_push_front(frame->vars, var);
}

void generateFrameTrace(frame* frame)
{
	if(frame==NULL)
		return;

	// Frame header's size
	DbgPrintf("<frame l=\"%i\" id=\"%s\">", frame->line, frame->name);

	// Variables' size
	linked_list* f_vars=frame_get_variables(frame);
	int varCount=linked_list_length(f_vars);

	for(int i=0; i<varCount; ++i)
	{
		print_variable((variable*)f_vars->data);
		f_vars=f_vars->next;
	}

	Serial.print("</frame>");
}
