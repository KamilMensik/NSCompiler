#ifndef TOKEN_H
#define TOKEN_H

#include "stack.h"

enum KEYWORDS {
    LET = 0,
    CONST = 1,
    FUN = 2,
    IF = 3,
    ELSE = 4,
    WHILE = 5,
    RETURN = 6,
    INCLUDE = 7,
};

enum TOKEN_TYPES {
    TOKEN_UNARY_OPERATOR,
    TOKEN_BINARY_OPERATOR,
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_DIVIDER,
    TOKEN_ENDLINE,
    TOKEN_PARENTHESIS,
    TOKEN_SQUARE_BRACKET,
    TOKEN_CURLY_BRACE,
    TOKEN_SEMICOLON,
    TOKEN_EOF,
    TOKEN_KEYWORD,
    TOKEN_DATA_TYPE,
    TOKEN_STRING,
};

typedef struct TOKEN_STRUCT {
    int type;
    char *value;
    union {
        int keyword_id;
        int data_type_id;
    };
    int line;
    int char_index;
} token_T;

token_T *init_token(int type, char *value, int line, int char_index, char should_free_value);

void init_keywords();

void free_keywords();

token_T *token_pop(stack_T *stack);

token_T *token_peek(stack_T *stack);

void token_push(stack_T *stack, token_T *token);

token_T **token_stack_to_array(stack_T *stack, int keep_stack);

void print_token(token_T *token);

int token_data_type(token_T *token);

void free_token(token_T *token);

#endif
