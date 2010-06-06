#ifndef VARIABLE_H
#define VARIABLE_H

	typedef enum {_error=-1, _int=0, _unsigned_int=1, _char=2, _unsigned_char=3, _float=4, _double=5, _char_pointer=6, _void_pointer=7}variable_type;

	typedef struct
	{
		char* name;
		variable_type type;
		int size;
		void* data;
	}variable;
	
	const char* variable_type_to_string(variable_type type)
	{
		const char* ret;
		switch(type)
		{
			case _error:{ret="error"; break;}
			case _int:{ret="int"; break;}
			case _unsigned_int:{ret="unsigned int"; break;}
			case _char:{ret="char"; break;}
			case _unsigned_char:{ret="unsigned char"; break;}
			case _float:{ret="float"; break;}
			case _double:{ret="double"; break;}
			case _char_pointer:{ret="char*"; break;}
			case _void_pointer:{ret="void*"; break;}
		}
		return ret;
	}
	
	variable_type variable_type_from_string(const char* type)
	{
		if(strcmp(type, "int")==0)
			return _int;
		else if(strcmp(type, "unsigned int")==0)
			return _unsigned_int;
		else if(strcmp(type, "char")==0)
			return _char;
		else if(strcmp(type, "unsigned char")==0)
			return _unsigned_char;
		else if(strcmp(type, "float")==0)
			return _float;
		else if(strcmp(type, "double")==0)
			return _double;
		else if(strcmp(type, "char*")==0)
			return _char_pointer;
		else if(strcmp(type, "void*")==0)
			return _void_pointer;
		else
			return _error;
	}
	
	variable* variable_create(const char* name, variable_type type, int size, void* data)
	{
		variable* var = (variable*)malloc(sizeof(variable));
		
		var->name=strdup(name);
		var->type=type;
		var->size=size;
		var->data=data;
		
		return var;
	}
	
	void variable_free(void* var)
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
		
		const char* s_type=variable_type_to_string(var->type);
		
		int size=strlen(s_type)+1+strlen(var->name)+3+1+1;
		
		// Get the pattern
		char* ret;
		const char* pattern;
		switch(var->type)
		{
			case _int:
			{
				size+=2+10;
				ret=(char*)malloc(size);
				snprintf(ret, size, "%s %s = %i;", s_type, var->name, *((int*)var->data));
				break;
			}
			case _unsigned_int:
			{
				size+=2+10;
				ret=(char*)malloc(size);
				snprintf(ret, size, "%s %s = %u;", s_type, var->name, *((unsigned int*)var->data));
				break;
			}
			case _char:
			{
				size+=2+1;
				ret=(char*)malloc(size);
				snprintf(ret, size, "%s %s = %c;", s_type, var->name, *((char*)var->data));
				break;
			}
			case _unsigned_char:
			{
				size+=2+1;
				ret=(char*)malloc(size);
				snprintf(ret, size, "%s %s = %c;", s_type, var->name, *((unsigned char*)var->data));
				break;
			}
			case _float:
			{
				size+=2+15;
				ret=(char*)malloc(size);
				snprintf(ret, size, "%s %s = %f;", s_type, var->name, *((float*)var->data));
				break;
			}
			case _double:
			{
				size+=2+15;
				ret=(char*)malloc(size);
				snprintf(ret, size, "%s %s = %e;", s_type, var->name, *((double*)var->data));
				break;
			}
			case _char_pointer:
			{
				size+=2+strlen(*((char**)var->data))+2;
				ret=(char*)malloc(size);
				snprintf(ret, size, "%s %s = '%s';", s_type, var->name, *((char**)var->data));
				break;
			}
			case _error:
			case _void_pointer:
			{
				size+=2+strlen((char*)var->data)+2;
				ret=(char*)malloc(size);
				snprintf(ret, size, "%s %s = %p;", s_type, var->name, var->data);
				break;
			}
		}

		return ret;
	}
#endif
