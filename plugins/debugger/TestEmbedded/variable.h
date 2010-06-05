#ifndef VARIABLE_H
#define VARIABLE_H

	typedef struct
	{
		char* name;
		int size;
		void* data;
	}variable;
	
	variable* variable_create(const char* name, int size, void* data)
	{
		variable* var = (variable*)malloc(sizeof(variable));
		
		var->name=strdup(name);
		var->size=size;
		var->data=data;
		
		return var;
	}
	
	variable_free(void* var)
	{
		
		if(var)
		{
			free(((variable*)var)->name);
			free(var);
		}
	}
	
	char* show_variable(void* data)
	{
		variable* var=(variable*)data;
		
		if(var==NULL)
			return NULL;
		
		int size=(1+strlen(var->name)+1)+1+(1+2+sizeof(void*)*2)+1+1;
		char* ret=(char*)malloc(size);
		
		int pos=snprintf(ret, size, "'%s'='%p'", var->name, var->data);

		return ret;
	}
#endif
