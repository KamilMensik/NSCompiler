#include "include/ast.h"
#include "include/token.h"
#include <stdio.h>
#include <stdlib.h>

ast_T *init_ast(int type, int subtype) {
    ast_T *ast = malloc(sizeof(struct AST_STRUCT));
    ast->type = type;
    ast->subtype = subtype;

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

ast_T **ast_stack_to_array(stack_T *stack, int keep_stack) {
    ast_T **asts = malloc(sizeof(struct AST_STRUCT *) * stack->size);

    for (int i = stack->size - 1; i >= 0; i--) {
        asts[i] = ast_pop(stack);
    }

    if (!keep_stack) free(stack);

    return asts;
}

/*
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
        for (int i = 0; i < ast->children_count; i++) {
            print_ast(ast->children[i]);
            printf(" ");
        }
        printf(")");
    }
    printf(")");
}
*/
