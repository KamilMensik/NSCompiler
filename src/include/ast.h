#ifndef AST_H
#define AST_H
#include "token.h"
#include "stack.h"
#include "defined_functions.h"

typedef struct AST_STRUCT ast_T;
struct AST_STRUCT {
    enum {
        AST_START,
        AST_COMPOUND,
        AST_DEFINITION,
        AST_REGION,
        AST_ATOM,
        AST_CONDITIONAL
    } type;
    unsigned int size_bytes;
    unsigned int children_count;
    unsigned short additional_data;
    enum {
        ADDITIONAL_DATA_BEHIND,
        ADDITIONAL_DATA_INSIDE
    } additional_data_position;
    unsigned int output_count;
    token_T *token;
    ast_T **children;
    defined_function_T *assigned_defined_function;
    unsigned int selected_defined_function_variation;
};

ast_T *init_ast(int type, token_T *token);

ast_T *ast_pop(stack_T *stack);

ast_T *ast_peek(stack_T *stack);

void ast_push(stack_T *stack, ast_T *ast);

void print_ast(ast_T *ast);
#endif