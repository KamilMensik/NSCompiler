#ifndef PARSER_H
#define PARSER_H
#include "lexer.h"
#include "token.h"
#include "ast.h"
#include "stack.h"

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
    token_T *token;
    stack_T *stack;
    ast_T *ast_root;
    int base_state;
    int advanced_state;

} parser_T;

parser_T *init_parser(lexer_T *lexer);

void handle_normal_function(parser_T *parser);

void handle_conditional_function(parser_T *parser);

list_T *get_tokens_until_character(lexer_T *lexer, char c, token_T *token) ;

list_T *get_tokens_inside_brackets(lexer_T *lexer);

ast_T *parse_expression(list_T *tokens);

ast_T *parse_statement(lexer_T *lexer, token_T *token);

ast_T *parse_function_definition(lexer_T *lexer);

ast_T *parse_variable_definition(lexer_T *lexer, int type);

ast_T *parse_uncertain(lexer_T *lexer);

ast_T *parser_parse(parser_T *parser);

#endif
