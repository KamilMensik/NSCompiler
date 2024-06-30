#include "include/parser.h"
#include <stdlib.h>
#include <stdio.h>

parser_T *init_parser(lexer_T *lexer) {
    parser_T *parser = calloc(1, sizeof(struct PARSER_STRUCT));
    parser->lexer = lexer;
    parser->token = lexer_get_next_token(lexer);
    parser->stack = init_ast_stack();
    ast_T *ast_root = init_ast(0, NULL);

    return parser;
}

void parser_handle_token(parser_T *parser) {
    ast_T *new_ast = NULL;
    switch (parser->token->type) {
        case TOKEN_NUMBER: case TOKEN_IDENTIFIER:
            new_ast = init_ast(AST_ATOM, parser->token);
            ast_push(parser->stack, new_ast);
            break;
        case TOKEN_FUNCTION: // Add functions with different parameter counts
            new_ast = init_ast(AST_COMPOUND, parser->token);
            new_ast->children = malloc(sizeof(struct AST_STRUCT*) * 2);
            new_ast->children_count = 2;
            new_ast->children[1] = ast_pop(parser->stack);
            new_ast->children[0] = ast_pop(parser->stack);
            ast_push(parser->stack, new_ast);
    }
}

ast_T *parser_parse(parser_T *parser) {
    while (parser->token != NULL) {
       parser_handle_token(parser);
       parser->token = lexer_get_next_token(parser->lexer);
    }

    return ast_pop(parser->stack);
}