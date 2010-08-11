#ifndef VARIABLE_H
#define VARIABLE_H

	typedef enum {_error=-1, _int=0, _unsigned_int=1, _char=2, _unsigned_char=3, _float=4, _double=5, _char_pointer=6, _void_pointer=7}variable_type;

	typedef struct
	{
        int line;
		char* name;
		variable_type type;
		int size;
		void* data;
	}variable;
	
	const char* variable_type_to_string(variable_type type);
	variable_type variable_type_from_string(const char* type);
	
	variable* variable_create(int line, const char* name, variable_type type, int size, void* data);
	
	void variable_set_value(variable* var, void* data, size_t size);
	void variable_set_value(variable* var, char* data);
	
	void variable_free(variable* var);
	void variable_free(void* var);

	char* show_variable(void* data);
	void print_variable(variable* var);
#endif
