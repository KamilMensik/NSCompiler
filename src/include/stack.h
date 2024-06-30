#ifndef STACK_H
#define STACK_H

#include "ast.h"

typedef struct AST_STACK_NODE_STRUCT ast_stack_node_T;
struct AST_STACK_NODE_STRUCT {
    ast_T *ast;
    ast_stack_node_T *next;
};

typedef struct AST_STACK_STRUCT {
    ast_stack_node_T *top;
    unsigned int size;
} ast_stack_T;

ast_stack_T *init_ast_stack();

ast_stack_node_T *init_ast_stack_node(ast_T *ast);

ast_T *ast_pop(ast_stack_T *stack);

ast_T *ast_peek(ast_stack_T *stack);

void ast_push(ast_stack_T *stack, ast_T *ast);

#endif