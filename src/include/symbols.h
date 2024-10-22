#ifndef SYMBOLS_H
#define SYMBOLS_H

#include "hashmap.h"
#include "data_types.h"
#include "token.h"
#include "list.h"
#include <stdio.h>

enum SYMBOL_TYPE {
    function,
    constant,
    variable
};

typedef struct FUNCTION_STRUCT {
    unsigned char return_type;
    unsigned char param_count;
    unsigned char *param_datatypes;
    hashmap_T *variables;
    list_T *memory_assignment;
    unsigned long memory_assignment_start;
    unsigned int memory_assignment_end;
    unsigned int address;
} function_T;

typedef struct CONSTANT_STRUCT {
    unsigned char type;
    unsigned int line;
    void *expression;
} constant_T;

typedef struct VARIABLE_STRUCT {
    function_T *context;
    unsigned char global;
    unsigned char type;
    unsigned int line;
    union {
        unsigned short offset;
        unsigned short address;
    };
} variable_T;

typedef struct SYMBOL_STRUCT {
    unsigned char type;
    union {
        function_T *function;
        variable_T *variable;
        constant_T *constant;
    };

} symbol_T;

void init_symbols();

symbol_T *get_symbol(char *name, char *context);

void set_symbol(char *name, char *context, symbol_T *sym);

symbol_T *init_function(char *name, unsigned char return_type, unsigned int line, unsigned int param_count, token_T **params);

symbol_T *init_variable(char *name, unsigned char type, unsigned int line, char *context);

symbol_T *init_constant(char *name, void *expression, unsigned char type, unsigned int line, char *context);
#endif
