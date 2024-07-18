#include "include/list.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

ast_list_T *init_ast_list(int size) {
    ast_list_T *list = malloc(sizeof(struct AST_LIST_STRUCT));
    list->size = size;
    list->top = 0;
    list->array = malloc(sizeof(ast_T *) * size);

    return list;
}

void ast_list_push(ast_list_T *list, ast_T *item) {
    if (list->size == (list->top - 1)) {
        list->size *= 2;
        realloc(list->array, list->size * sizeof(ast_T *));
    }

    list->array[list->top] = item;
    list->top += 1;
}

token_list_T *init_token_list(int size) {
    token_list_T *list = malloc(sizeof(struct TOKEN_LIST_STRUCT));
    list->size = size;
    list->top = 0;
    list->array = malloc(sizeof(token_T *) * size);

    return list;
}

void token_list_push(token_list_T *list, token_T *item) {
    if (list->size == (list->top - 1)) {
        list->size *= 2;
        realloc(list->array, list->size * sizeof(token_T *));
    }

    list->array[list->top] = item;
    list->top += 1;
}