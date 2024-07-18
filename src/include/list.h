#ifndef LIST_H
#define LIST_H

typedef struct AST_LIST_STRUCT ast_list_T;
typedef struct TOKEN_LIST_STRUCT token_list_T;

#include "ast.h"
#include "token.h"

struct AST_LIST_STRUCT {
    unsigned long top;
    unsigned long size;
    ast_T **array;
};

struct TOKEN_LIST_STRUCT {
    unsigned long top;
    unsigned long size;
    token_T **array;
};

ast_list_T *init_ast_list(int size);

void ast_list_push(ast_list_T *list, ast_T *item);

token_list_T *init_token_list(int size);

void token_list_push(token_list_T *list, token_T *item);

#endif