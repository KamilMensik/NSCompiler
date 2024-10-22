#ifndef PARSER_H
#define PARSER_H
#include "lexer.h"
#include "token.h"
#include "ast.h"
#include "stack.h"
#include "hashmap.h"

enum PARSER_STATES {
    PARSING_UNCERTAIN,
    PARSING_FUNCTION,
    PARSING_VARIABLE
};

enum UNCERTAIN_STATES {
    UNCERTAIN_NOTHING,
    UNCERTAIN_ONE_IDENTIFIER,
    UNCERTAIN_TWO_IDENTIFIERS
};

typedef struct PARSER_STRUCT {
    lexer_T *lexer;
    char *context;
} parser_T;

parser_T *init_parser(lexer_T *lexer);

void free_parser(parser_T *parser, int final);

void handle_normal_function(parser_T *parser);

void handle_conditional_function(parser_T *parser);

list_T *get_tokens_until_character(lexer_T *lexer, char c, token_T *token) ;

list_T *get_tokens_inside_brackets(lexer_T *lexer);

ast_T *parse_expression(parser_T *parser, list_T *tokens);

ast_T *parse_statement(parser_T *parser, token_T *token);

ast_T *parse_function_definition(parser_T *parser);

ast_T *parse_variable_definition(parser_T *parser, int type);

ast_T *parse_uncertain(parser_T *parser);

ast_T *parser_parse(parser_T *parser);

#endif
