#include "include/parser.h"
#include "include/defined_functions.h"
#include "include/hashmap.h"
#include <stdlib.h>
#include <stdio.h>

parser_T *init_parser(lexer_T *lexer) {
    parser_T *parser = calloc(1, sizeof(struct PARSER_STRUCT));
    parser->lexer = lexer;
    parser->token = lexer_get_next_token(lexer);
    parser->stack = init_stack();

    return parser;
}

void parser_handle_token(parser_T *parser) {
    ast_T *new_ast = NULL;
    switch (parser->token->type) {
        case TOKEN_NUMBER: case TOKEN_IDENTIFIER:
            new_ast = init_ast(AST_ATOM, parser->token);
            new_ast->output_count = 1;
            ast_push(parser->stack, new_ast);
            break;
        case TOKEN_FUNCTION:
            new_ast = init_ast(AST_COMPOUND, parser->token);
            new_ast->output_count = defined_functions_hashmap_get(parser->token->value)->output_count;
            int parameter_count = defined_functions_hashmap_get(parser->token->value)->parameter_count;
            ast_T **max_children_array = malloc(sizeof(struct AST_STRUCT *) * parameter_count);
            int i = 0;
            for (;;i++) {
                if (parameter_count == 0)
                    break;
                ast_T *child = ast_pop(parser->stack);
                parameter_count -= child->output_count;
                max_children_array[i] = child;
            }
            new_ast->children = malloc(sizeof(struct AST_STRUCT*) * i);
            for (int j = 0; j < i; j++) {
                new_ast->children[i-j-1] = max_children_array[j];
            }
            new_ast->children_count = i;
            free(max_children_array);
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