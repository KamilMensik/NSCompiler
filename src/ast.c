#include "include/ast.h"
#include "include/token.h"
#include <stdio.h>
#include <stdlib.h>

ast_T *init_ast(int type, token_T *token) {
    ast_T *ast = calloc(1, sizeof(struct AST_STRUCT));
    ast->type = type;
    ast->size_bytes = 0;
    ast->children_count = 0;
    ast->children = NULL;
    ast->token = token;
    ast->additional_data = 0;
    ast->output_count = 0;

    return ast;
}

ast_T *ast_pop(stack_T *stack) {
    return (struct AST_STRUCT *)(stack_pop(stack));
}

ast_T *ast_peek(stack_T *stack) {
    return (struct AST_STRUCT *)(stack_peek(stack));
}

void ast_push(stack_T *stack, ast_T *ast) {
    stack_push(stack, ast);
}

void print_ast(ast_T *ast) {
    char *readable_type;
    switch (ast->type) {
        case AST_ATOM:
            readable_type = "ATOM";
            break;
        case AST_COMPOUND:
            readable_type = "COMPOUND";
            break;
        case AST_START:
            readable_type = "START";
            break;
        case AST_CONDITIONAL:
            readable_type = "CONDITIONAL";
            break;
        case AST_DEFINITION:
            readable_type = "DEFINITION";
            break;
        case AST_REGION:
            readable_type = "REGION";
            break;
    }

    printf("(AST %s", readable_type);
    if (ast->token != NULL) {
        printf(" ");
        print_token(ast->token);
    }
    if (ast->children_count > 0) {
        printf(" (CHILDREN ");
        for (int i = 0; i < ast->children_count; i++)
            print_ast(ast->children[i]);
            printf(" ");
        printf(")");
    }
    printf(")\n");
}