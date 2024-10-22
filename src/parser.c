#include "include/parser.h"
#include "include/ast.h"
#include "include/defined_functions.h"
#include "include/lexer.h"
#include "include/list.h"
#include "include/error.h"
#include "include/stack.h"
#include "include/token.h"
#include "include/symbols.h"
#include "include/hashmap.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

hashmap_T *visited_files = NULL;

parser_T *init_parser(lexer_T *lexer) {
    if (!visited_files)
        visited_files = new_hashmap(256);

    parser_T *parser = calloc(1, sizeof(struct PARSER_STRUCT));
    parser->lexer = lexer;

    // Add file to visited, so that we dont parse the same file twice
    char *visited = malloc(sizeof(char));
    *visited = 1;
    hashmap_set(visited_files, lexer->filename, visited);

    return parser;
}

void free_parser(parser_T *parser, int final) {
    if (final) {
        free_hashmap(visited_files, 1);
    }
    free_lexer(parser->lexer);
    free(parser);
}

list_T *get_tokens_until_character(lexer_T *lexer, char c, token_T *token) {
    list_T *list = init_list(8);
    
    if (token==NULL)
        token = lexer_get_next_token(lexer);

    while (token->value[0] != c) {
        list_push(list, token);
        token = lexer_get_next_token(lexer);

        if (token->type == TOKEN_EOF) {
            printf("You missed %c somewhere in the code!", c);
            exit(1);
        }
    }

    free_token(token);
    return list;
}

void add_to_symbol_table(hashmap_T *symbol_table, char *key, void *item) {
    if (hashmap_get(symbol_table, key) != NULL)
        throw_error("Duplicate symbol");

    hashmap_set(symbol_table, key, item);
}

// Handle parenthesis depth, returns 1 if depth is completed
int handle_parenthesis_depth(stack_T *depth, token_T *token) {
    token_T *checked_token;
    switch (token->value[0]) {
        case '(': case '{': case '[':
            stack_push(depth, token);
            break;
        case ')':
            checked_token = token_pop(depth);
            if (checked_token->value[0] != '(') 
                throw_error("Bad bracket");
            break;
        case '}':
            checked_token = token_pop(depth);
            if (checked_token->value[0] != '{') 
                throw_error("Bad bracket");
            break;
        case ']':
            checked_token = token_pop(depth);
            if (checked_token->value[0] != '[') 
                throw_error("Bad bracket");
            break;
    }
    
    if (depth->size == 0) {
        free_token(token);
        free_token(checked_token);
        return 1;
    }

    return 0;
}

list_T *get_tokens_inside_brackets(lexer_T *lexer) {
    stack_T *branching = init_stack(); 
    list_T *result = init_list(8);
    token_T *token = lexer_get_next_token(lexer);

    if ((token->type != TOKEN_PARENTHESIS && token->type != TOKEN_CURLY_BRACE && token->type != TOKEN_SQUARE_BRACKET) &&
        token->value[0] != '(' && token->value[0] != '{' && token->value[0] != '[')
        throw_token_error(token, "Expected bracket");
    
    token_push(branching, token);
    while(1) {
        token = lexer_get_next_token(lexer);

        if ((token->type == TOKEN_PARENTHESIS || token->type == TOKEN_CURLY_BRACE || token->type == TOKEN_SQUARE_BRACKET) &&
            handle_parenthesis_depth(branching, token)) {
                break;
        } else {
            list_push(result, token);
        }
    }
    
    free_stack(branching);
    return result;
}

token_T *expression_get_safe(list_T *tokens, int i) {
    if (tokens->top <= i) return NULL;

    return (token_T *)(tokens->array[i]);
}

ast_T *parse_expression(parser_T *parser, list_T *tokens) {
    list_T *expressions = init_list(8);
    stack_T *operators = init_stack();
    stack_T *depth;
    
    for (int i = 0;;) {
        token_T *token = expression_get_safe(tokens, i);
        if (token==NULL) break;

        ast_T *expression = init_ast(EXPRESSION, 0);
        switch (token->type) {
            case TOKEN_NUMBER:
                expression->subtype = EXPRESSION_NUMBER;
                expression->params.literal_expression_params.token = token;
                list_push(expressions, expression);
                i++;
                break;
            case TOKEN_STRING:
                expression->subtype = EXPRESSION_STRING;
                expression->params.literal_expression_params.token = token;
                list_push(expressions, expression);
                i++;
                break;
            case TOKEN_IDENTIFIER:
                expression->subtype = EXPRESSION_IDENTIFIER;
                expression->params.literal_expression_params.token = token;
                if (expression_get_safe(tokens, i+1) && ((token_T *)(tokens->array[i + 1]))->type == TOKEN_PARENTHESIS) {
                    symbol_T *symbol = get_symbol(token->value, NULL);
                    if (symbol == NULL || symbol->type != function)
                        throw_token_error(token, "Symbol is not a function");

                    token = tokens->array[++i];
                    if (token->value[0] != '(')
                        throw_token_error(token, "Expression error, expected '('.");

                    ast_T *funcall_expression = init_ast(EXPRESSION, EXPRESSION_FUNCALL);
                    funcall_expression->symbol = symbol;
                    stack_T *arguments = init_stack();
                    depth = init_stack();
                    stack_push(depth, token);

                    list_T *expression_tokens = init_list(8);
                    int finished = 0;

                    while (!finished) {
                        token = expression_get_safe(tokens, ++i);
                        if (!token)
                            throw_error("Expression error, unfinished expression");

                        switch (token->type) {
                            case TOKEN_PARENTHESIS: case TOKEN_CURLY_BRACE: case TOKEN_SQUARE_BRACKET:
                                if (handle_parenthesis_depth(depth, token)) {
                                    free_stack(depth);
                                    if (expression_tokens->top > 0)
                                        stack_push(arguments, parse_expression(parser, expression_tokens));
                                    finished = 1;
                                    i++;
                                } else {
                                    list_push(expression_tokens, token);
                                }
                                break;
                            case TOKEN_DIVIDER:
                                free_token(token);
                                if (depth->size == 1) {
                                    // TODO: FIX POTENTIAL MEMORY LEAK
                                    stack_push(arguments, parse_expression(parser, expression_tokens));
                                    expression_tokens = init_list(8);
                                } else {
                                    list_push(expression_tokens, token);
                                }
                                break;
                            default:
                                list_push(expression_tokens, token);
                                break;
                        }
                    }
                    if (arguments->size != symbol->function->param_count) {
                        printf("Unexpected number of arguments when calling function %s. Expected %d, got %d\n",
                               expression->params.literal_expression_params.token->value,
                               symbol->function->param_count,
                               arguments->size);
                        exit(1);
                    }
                    funcall_expression->params.funcall_expression_params.function_expression = expression;
                    funcall_expression->params.funcall_expression_params.param_expressions_size = arguments->size;
                    funcall_expression->params.funcall_expression_params.param_expressions = ast_stack_to_array(arguments, 0);
                    list_push(expressions, funcall_expression);
                } else if (expression_get_safe(tokens, i+1) && ((token_T *)(tokens->array[i + 1]))->type == TOKEN_SQUARE_BRACKET) {
                    token = tokens->array[++i];
                    if (token->value[0] != '[')
                        throw_token_error(token, "Expression error, expected '['.");

                    list_T *token_list = init_list(8);
                    depth = init_stack();
                    token_push(depth, token);
                    int finished = 0;

                    while(!finished) {
                        if (++i >= tokens->top)
                            throw_error("Expression error, unfinished expression");

                        token = tokens->array[i];
                        switch (token->type) {
                            case TOKEN_PARENTHESIS: case TOKEN_CURLY_BRACE: case TOKEN_SQUARE_BRACKET:
                                if (handle_parenthesis_depth(depth, token)) {
                                    if (token_list->size == 0)
                                        throw_token_error(token, "Expression error, expected indexing expression.");

                                    free_stack(depth);
                                    ast_T *indexing_expression = init_ast(EXPRESSION, EXPRESSION_INDEXING);
                                    indexing_expression->params.indexing_expression_params.arrray_expression = expression;
                                    indexing_expression->params.indexing_expression_params.index_expression = parse_expression(parser, token_list);
                                    list_push(expressions, indexing_expression);
                                    finished = 1;
                                    i++;
                                } else {
                                    list_push(token_list, token);
                                }
                                break;
                            default:
                                list_push(token_list, token);
                                break;
                        }
                    }
                } else {
                    symbol_T *symbol = get_symbol(expression->params.literal_expression_params.token->value, parser->context);
                    if (symbol == NULL)
                        throw_token_error(expression->params.literal_expression_params.token, "Unknown symbol.");
                    else if (symbol->type == function)
                        throw_token_error(expression->params.literal_expression_params.token, "Symbol is a function, not a variable or constant.");

                    expression->symbol = symbol;
                    list_push(expressions, expression);
                    i++;
                }

                break;
            case TOKEN_PARENTHESIS:
                if (token->value[0] == ')')
                    throw_token_error(token, "Expression error, unexpected parenthesis detected.");

                list_T *tokens_inside_parenthesis = init_list(8);
                depth = init_stack();
                stack_push(depth, token);
                int finished = 0;

                while(!finished) {
                    if (++i >= tokens->top)
                        throw_error("Expression error, unfinished expression");

                    token = tokens->array[i];
                    switch (token->type) {
                        case TOKEN_PARENTHESIS: case TOKEN_CURLY_BRACE: case TOKEN_SQUARE_BRACKET:
                            if (handle_parenthesis_depth(depth, token)) {
                                if (tokens_inside_parenthesis->size == 0)
                                    throw_token_error(token, "Expression error, expected indexing expression.");

                                free_stack(depth);
                                finished = 1;
                                i++;
                            } else {
                                list_push(tokens_inside_parenthesis, token);
                            }
                            break;
                        default:
                            list_push(tokens_inside_parenthesis, token);
                            break;
                    }
                }

                free_ast(expression, 1);
                expression = parse_expression(parser, tokens_inside_parenthesis);
                list_push(expressions, expression);
                break;
            case TOKEN_UNARY_OPERATOR:
                if (expressions->top < 1)
                    throw_token_error(token, "Unary operator is without an expression before it");

                expression->subtype = EXPRESSION_UNARY_OP;
                expression->params.unary_op_expression_params.expression = (ast_T *)list_pop(expressions);
                expression->params.unary_op_expression_params.op = token;
                list_push(expressions, expression);
                i++;
                break;
            case TOKEN_BINARY_OPERATOR:
                if (expressions->top < 1)
                    throw_token_error(token, "Binary operator is without an expression before it");

                if (operators->size > 0 && operator_priority(token_peek(operators)->value) <= operator_priority(token->value)) {
                    if (expressions->top < 2)
                        throw_token_error(token, "Binary operator is missing second expression");

                    expression->subtype = EXPRESSION_BINARY_OP;
                    expression->params.binary_op_expression_params.op = token_pop(operators);
                    expression->params.binary_op_expression_params.r_expression = (ast_T *)list_pop(expressions);
                    expression->params.binary_op_expression_params.l_expression = (ast_T *)list_pop(expressions);
                    list_push(expressions, expression);
                } else {
                    free(expression);
                }
                
                i++;
                token_push(operators, token);
                break;
            default:
                throw_token_error(token, "Unexpected token");
                break;
        }
    }
    free_list(tokens);

    while (operators->size != 0) {
        token_T *op = token_pop(operators);
        if (expressions->top < 2)
            throw_token_error(op, "Operator missing its expressions");

        ast_T *binary_expression = init_ast(EXPRESSION, EXPRESSION_BINARY_OP);
        binary_expression->params.binary_op_expression_params.op = op;
        binary_expression->params.binary_op_expression_params.r_expression = (ast_T *)list_pop(expressions);
        binary_expression->params.binary_op_expression_params.l_expression = (ast_T *)list_pop(expressions);
        list_push(expressions, binary_expression);
    }

    free_stack(operators);
    ast_T *final_expression = expressions->top == 0 ? NULL : list_pop(expressions);
    free_list(expressions);
    return final_expression;
}

ast_T *parse_statement(parser_T *parser, token_T *token) {
    lexer_T *lexer = parser->lexer;

    if (token == NULL)
        token = lexer_get_next_token(lexer);
    ast_T *statement = init_ast(STATEMENT, 0);
    stack_T *stack = NULL;
    list_T *expression_tokens = NULL;
    int tmp_size;
    int depth;

    switch (token->type) {
        case TOKEN_SEMICOLON:
            statement->subtype = STATEMENT_NOP;
            break;
        case TOKEN_KEYWORD:
            switch (token->keyword_id) {
                case IF:
                    statement->subtype = STATEMENT_CONDITIONAL;
                    free_token(token);
                    statement->params.conditional_statement_params.condition_expression = parse_expression(parser, get_tokens_inside_brackets(lexer));
                    statement->params.conditional_statement_params.if_block_statement = parse_statement(parser, NULL);
                    statement->params.conditional_statement_params.else_block_statement = NULL;
                    token = lexer_peek(lexer);
                    if (token->type == TOKEN_KEYWORD && token->keyword_id == ELSE) {
                        lexer_get_next_token(lexer);
                        free_token(token);
                        statement->params.conditional_statement_params.else_block_statement = parse_statement(parser, NULL);
                    }

                    break;
                case WHILE:
                    statement->subtype = STATEMENT_LOOP;
                    free_token(token);
                    statement->params.loop_statement_params.condition_expression = parse_expression(parser, get_tokens_inside_brackets(lexer));
                    statement->params.loop_statement_params.statement = parse_statement(parser, NULL);
                    break;
                case RETURN:
                    statement->subtype = STATEMENT_RETURN;
                    free_token(token);
                    statement->params.regular_expression_statement_params.expression = parse_expression(parser, get_tokens_until_character(lexer, ';', NULL));
                    break;
                case LET:
                    free_ast(statement, 1);
                    free_token(token);
                    statement = parse_variable_definition(parser, DEFINITION_VARIABLE);
                    statement->type = STATEMENT;
                    statement->subtype = STATEMENT_VARIABLE_DECLARATION;
                    break;
                default:
                    throw_token_error(token, "Statement error, unexpected keyword inside function.");
            }
            break;
        case TOKEN_CURLY_BRACE:
            if (token->value[0] != '{')
                throw_token_error(token, "Statement error, expected '{'.");
            
            stack = init_stack();
            statement->subtype = STATEMENT_COMPOUND;
            free_token(token);
            token = lexer_get_next_token(lexer);

            while (token->type != TOKEN_CURLY_BRACE && token->value[0] != '}') {
                ast_push(stack, parse_statement(parser, token));
                token = lexer_get_next_token(lexer);
            }
            free_token(token);

            statement->params.compound_statement_params.statement_count = stack->size;
            statement->params.compound_statement_params.statements = ast_stack_to_array(stack, 0);
            break;
        default:
            statement->subtype = STATEMENT_EXPRESSION;
            statement->params.regular_expression_statement_params.expression = parse_expression(parser, get_tokens_until_character(lexer, ';', token));
            break;
    }

    return statement;
}

ast_T *parse_function_definition(parser_T *parser) {
    lexer_T *lexer = parser->lexer;
    ast_T *ast = init_ast(DEFINITION, DEFINITION_FUNCTION);
    token_T *token = lexer_get_next_token(lexer);
    stack_T *stack = init_stack();

    if (token->type != TOKEN_DATA_TYPE)
        throw_token_error(token, "Function definition error, expected data type.");

    ast->params.function_definition_params.type = token;
    
    token = lexer_get_next_token(lexer);
    if (token->type != TOKEN_IDENTIFIER)
        throw_token_error(token, "Function definition error, expected identifier as name of function.");

    ast->params.function_definition_params.name = token;
    parser->context = token->value;

    token = lexer_get_next_token(lexer);
    if (token->type != TOKEN_PARENTHESIS && token->value[0] != '(')
        throw_token_error(token, "Function definition error, expected '('.");
    
    free_token(token);

    // Get parameters
    token = lexer_get_next_token(lexer);
    if (token->value[0] != ')') {
        while (1) {
            if (token->type != TOKEN_DATA_TYPE)
                throw_token_error(token, "Function definition error, expected parameters or closing parenthesis.");

            token_push(stack, token);

            token = lexer_get_next_token(lexer);
            if (token->type != TOKEN_IDENTIFIER)
                throw_token_error(token, "Function definition error, expected identifier as parameter name.");

            token_push(stack, token);

            token = lexer_get_next_token(lexer);
            
            if(token->type == TOKEN_DIVIDER) {
                free_token(token);
                token = lexer_get_next_token(lexer);
            } else if (token->type == TOKEN_PARENTHESIS && token->value[0] == ')') {
                free_token(token);

                ast->params.function_definition_params.parameters_count = stack->size / 2;
                ast->params.function_definition_params.parameters = token_stack_to_array(stack, 0);
                break;
            } else {
                throw_token_error(token, "Function definition error, expected ',' or ')'.");
            }
        }
    } else {
        free_token(token);
        ast->params.function_definition_params.parameters_count = 0;
        ast->params.function_definition_params.parameters = NULL;
    }

    ast->symbol = init_function(ast->params.function_definition_params.name->value,
                                ast->params.function_definition_params.type->data_type_id,
                                ast->params.function_definition_params.name->line,
                                ast->params.function_definition_params.parameters_count,
                                ast->params.function_definition_params.parameters);

    ast->params.function_definition_params.statement = parse_statement(parser, NULL);
    parser->context = NULL;
    return ast;
}

ast_T *parse_variable_definition(parser_T *parser, int type) {
    ast_T *ast = init_ast(DEFINITION, type);
    lexer_T *lexer = parser->lexer;

    token_T *token = lexer_get_next_token(lexer);
    if (token->type != TOKEN_DATA_TYPE)
        throw_token_error(token, "Variable definition error, expected data type.");

    ast->params.variable_definition_params.type = token;
    
    token = lexer_get_next_token(lexer);
    if (token->type != TOKEN_IDENTIFIER)
        throw_token_error(token, "Variable definition error, expected identifier as name of function.");

    ast->params.variable_definition_params.name = token;

    token = lexer_get_next_token(lexer);
    if (token->type != TOKEN_BINARY_OPERATOR && strcmp(token->value, "=") != 0)
        throw_token_error(token, "Variable definition error, expected '='.");

    free_token(token);
    ast_T *expression = parse_expression(parser, get_tokens_until_character(lexer, ';', NULL));
    ast->params.variable_definition_params.expression = expression;

    if (type == DEFINITION_VARIABLE) {
        ast->symbol = init_variable(ast->params.variable_definition_params.name->value,
                                    ast->params.variable_definition_params.type->data_type_id,
                                    ast->params.variable_definition_params.name->line,
                                    parser->context);
    } else {
        ast->symbol = init_constant(ast->params.variable_definition_params.name->value,
                                    ast->params.variable_definition_params.expression,
                                    ast->params.variable_definition_params.type->data_type_id,
                                    ast->params.variable_definition_params.name->line,
                                    parser->context);
    }

    return ast;
}

ast_T *parse_file_include(parser_T *parser) {
    token_T *token = lexer_get_next_token(parser->lexer);
    if (!token || token->type != TOKEN_STRING)
        throw_token_error(token, "Error when trying to include another file. Expected a string.");

    int *is_file_visited = (int *)hashmap_get(visited_files, token->value);
    if (is_file_visited && *is_file_visited == 1) {
        // Free now irrelevant tokens
        free_token(token);
        token = lexer_get_next_token(parser->lexer);
        if (token->type != TOKEN_SEMICOLON)
            throw_token_error(token, "Error when including file. Expected ; to finish the statement.");
        free_token(token);

        ast_T *empty_programme = init_ast(PROGRAMME, 0);
        empty_programme->params.programme_params.function_definitions_count = 0;
        empty_programme->params.programme_params.variable_definitions_count = 0;
        return empty_programme;
    }

    char *file_name = malloc(sizeof(char) * (strlen(token->value) + 1));
    strcpy(file_name, token->value);
    FILE *included_file = fopen(file_name, "r");
    if (!included_file)
        throw_token_error(token, "Error when trying to include another file. File not found.");

    free_token(token);
    token = lexer_get_next_token(parser->lexer);
    if (token->type != TOKEN_SEMICOLON) {
        fclose(included_file);
        throw_token_error(token, "Error when including file. Expected ; to finish the statement.");
    }
    free_token(token);

    parser_T *new_parser = init_parser(init_lexer(included_file, file_name));
    ast_T *res = parser_parse(new_parser);
    fclose(included_file);
    free_parser(new_parser, 0);
    free(file_name);
    return res;
}

ast_T *parse_uncertain(parser_T *parser) {
    token_T *token = lexer_get_next_token(parser->lexer);

    switch (token->type) {
        case TOKEN_EOF:
            free_token(token);
            return NULL;
        case TOKEN_KEYWORD:
            switch(token->keyword_id) {
                case FUN:
                    free_token(token);
                    return parse_function_definition(parser);
                case LET:
                    free_token(token);
                    return parse_variable_definition(parser, DEFINITION_VARIABLE);
                case CONST:
                    free_token(token);
                    return parse_variable_definition(parser, DEFINITION_CONSTANT);
                case INCLUDE:
                    free_token(token);
                    return parse_file_include(parser);
                default:
                    throw_token_error(token, "Unexpected keyword. Expected LET, CONST or FUN.");
            }
    }

    throw_token_error(token, "Unexpected symbol, expected a keyword.");
    return NULL;
}

void append_programmes(stack_T *function_definitions, stack_T *variable_definitions, ast_T *secondary_programme) {
    struct PROGRAMME_PARAMS *params = &secondary_programme->params.programme_params;
    for (int i = 0; i < params->variable_definitions_count; i++) {
        ast_push(variable_definitions, params->variable_definitions[i]);
    }
    for (int i = 0; i < params->function_definitions_count; i++) {
        ast_push(function_definitions, params->function_definitions[i]);
    }
    free_ast(secondary_programme, 0);
}

ast_T *parser_parse(parser_T *parser) {
    stack_T *function_definitions = init_stack();
    stack_T *variable_definitions = init_stack();
    while (1) {
        ast_T *new_ast = parse_uncertain(parser);
        if (new_ast == NULL)
            break;

        if (new_ast->type == PROGRAMME)
            append_programmes(function_definitions, variable_definitions, new_ast);
        else
            ast_push(new_ast->subtype == DEFINITION_FUNCTION ? function_definitions : variable_definitions, new_ast);
    }

    ast_T *programme_ast = init_ast(PROGRAMME, 0);
    programme_ast->params.programme_params.function_definitions_count = function_definitions->size;
    programme_ast->params.programme_params.function_definitions = ast_stack_to_array(function_definitions, 0);
    programme_ast->params.programme_params.variable_definitions_count = variable_definitions->size;
    programme_ast->params.programme_params.variable_definitions = ast_stack_to_array(variable_definitions, 0);
    return programme_ast;
}
