#ifndef TOKEN_H
#define TOKEN_H

#include "hashmap.h"
#include "stack.h"

enum {
    LET = 0,
    CONST = 1,
    FUN = 2,
    IF = 3,
    ELSE = 4,
    WHILE = 5,
    RETURN = 6, 
    INT = 7
} KEYWORDS;

enum {
    TOKEN_UNARY_OPERATOR,
    TOKEN_BINARY_OPERATOR,
    TOKEN_ASSIGNMENT,
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_DIVIDER,
    TOKEN_ENDLINE,
    TOKEN_PARENTHESIS = 7,
    TOKEN_SQUARE_BRACKET = 8,
    TOKEN_CURLY_BRACE = 9,
    TOKEN_SEMICOLON,
    TOKEN_EOF,
    TOKEN_KEYWORD,
    TOKEN_FUNCTION, //REMOVE
    TOKEN_CONDITIONAL, //REMOVE
    TOKEN_REGION, //REMOVE
} TOKEN_TYPES;

typedef struct TOKEN_STRUCT {
    int type;
    char *value;
    int keyword_id;
    int line;
    int char_index;
} token_T;

token_T *init_token(int type, char *value, int line, int char_index);

hashmap_T *init_keywords();

token_T *token_pop(stack_T *stack);

token_T *token_peek(stack_T *stack);

void token_push(stack_T *stack, token_T *token);

token_T **token_stack_to_array(stack_T *stack, int keep_stack);

void print_token(token_T *token);

int token_data_type(token_T *token);

#endif