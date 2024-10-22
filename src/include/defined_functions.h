#ifndef DEFINED_FUNCTIONS_H
#define DEFINED_FUNCTIONS_H

extern char command_names[][10];

enum command_value_type {
    VALUE_INSIDE = 0,
    VALUE_OUTSIDE = 1,
};

typedef struct COMMAND_STRUCT {
    unsigned char code;
    unsigned char value_type;
    unsigned int value;
} command_T;

void free_command_codes();

unsigned int string_to_int(char *str);

int operator_priority(char *op);

unsigned char data_type_is_number(unsigned char data_type);

void handle_variable_assignment(void *ast);

void handle_unary_operator_expression(void *ast);

void handle_binary_operator_expression(void *ast);

unsigned char get_command_code(char *name);

#endif
