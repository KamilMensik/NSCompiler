#ifndef PARSER_H
#define PARSER_H
#include "lexer.h"
#include "token.h"
#include "ast.h"
#include "stack.h"
#include "hashmap.h"

typedef struct PARSER_STRUCT {
    lexer_T *lexer;
    token_T *token;
    stack_T *stack;
    ast_T *ast_root;
    hashmap_T *defined_functions_hashmap;
} parser_T;

parser_T *init_parser(lexer_T *lexer, hashmap_T *defined_functions_hashmap);

ast_T *parser_parse(parser_T *parser);

#endif