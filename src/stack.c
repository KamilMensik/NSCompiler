#include "include/stack.h"
#include <stdlib.h>

ast_stack_T *init_ast_stack() {
    ast_stack_T *stack = calloc(1, sizeof(struct AST_STACK_STRUCT));
    stack->top = NULL;

    return stack;
}

ast_stack_node_T *init_ast_stack_node(ast_T *ast) {
    ast_stack_node_T *node = calloc(1, sizeof(struct AST_STACK_NODE_STRUCT));
    node->ast = ast;

    return node;
}

ast_T *ast_pop(ast_stack_T *stack) {
    ast_stack_node_T *stack_node = stack->top;
    stack->top = stack_node->next;
    ast_T *ast = stack_node->ast;
    free(stack_node);
    stack->size -= 1;
    return ast;
}

ast_T *ast_peek(ast_stack_T *stack) {
    return stack->top->ast;
}

void ast_push(ast_stack_T *stack, ast_T *ast) {
    ast_stack_node_T *new_node = init_ast_stack_node(ast);
    new_node->next = stack->top;
    stack->top = new_node;
    stack->size += 1;
}