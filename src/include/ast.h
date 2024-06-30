#ifndef AST_H
#define AST_H
#include "token.h"
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
    token_T *token;
    ast_T **children;
};

ast_T *init_ast(int type, token_T *token);

void print_ast(ast_T *ast);
#endif