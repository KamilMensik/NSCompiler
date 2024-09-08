#include "include/parser.h"
#include "include/ast.h"
//#include "include/defined_functions.h"
//#include "include/hashmap.h"
#include "include/list.h"
#include "include/error.h"
#include "include/stack.h"
#include "include/token.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

parser_T *init_parser(lexer_T *lexer) {
    parser_T *parser = calloc(1, sizeof(struct PARSER_STRUCT));
    parser->lexer = lexer;
    parser->token = NULL;
    parser->stack = init_stack();

    return parser;
}

/*
void handle_normal_function(parser_T *parser) {
    ast_T *new_ast;
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

void handle_conditional_function(parser_T *parser) {
    int i = 0;
    unsigned int output_count = 0;
    ast_T *new_ast = NULL;
    ast_T **tmp;
    parser->token->type = TOKEN_CONDITIONAL;
    switch (parser->token->value[0]) {
        case 'i':
            new_ast = init_ast(AST_CONDITIONAL, parser->token);
            new_ast->children_count = 3;
            new_ast->children = calloc(3, sizeof(struct AST_STRUCT *));
            new_ast->children[0] = ast_pop(parser->stack);
            ast_push(parser->stack, new_ast);
            break;
        case 'e':
            new_ast = init_ast(AST_COMPOUND, parser->token);
            tmp = malloc(sizeof(struct AST_STRUCT *) * parser->stack->size);
            for (;; i++) {
                ast_T *child = ast_pop(parser->stack);
                if (child->type == AST_CONDITIONAL && child->children[1] == NULL) {
                    new_ast->children_count = i;
                    new_ast->children = malloc(sizeof(struct AST_STRUCT *) * i);
                    for (int j = 0; j < i; j++) {
                        new_ast->children[i - j - 1] = tmp[j];
                    }
                    child->output_count = output_count;
                    new_ast->output_count = output_count;
                    child->children[1] = new_ast;
                    ast_push(parser->stack, child);
                    break;
                } else {
                    output_count += child->output_count;
                    tmp[i] = child;
                }
            }
            free(tmp);
            break;
        case 't':
            new_ast = init_ast(AST_COMPOUND, parser->token);
            tmp = malloc(sizeof(struct AST_STRUCT *) * parser->stack->size);
            for (;; i++) {
                ast_T *child = ast_pop(parser->stack);
                if (child->type == AST_CONDITIONAL && child->children[2] == NULL) {
                    new_ast->children_count = i;
                    new_ast->children = malloc(sizeof(struct AST_STRUCT *) * i);
                    for (int j = 0; j < i; j++) {
                        new_ast->children[i - j - 1] = tmp[j];
                    }
                    new_ast->output_count = output_count;
                    if (output_count != child->children[1]->output_count) {
                        printf("Error during parsing, if/else branch has output of different size\n");
                        exit(1);
                    }
                    child->children[2] = new_ast;
                    ast_push(parser->stack, child);
                    break;
                } else {
                    output_count += child->output_count;
                    tmp[i] = child;
                }
            }
            free(tmp);
            break;
    }
}

// Variable declaration AST node => (*const type name value), it always ends with \n
ast_T *handle_data_region(parser_T *parser) {
    ast_T *data_region = init_ast(AST_REGION, parser->token);
    ast_T **data_region_children_temp = malloc(sizeof(struct AST_STRUCT *) * 65536);
    int data_region_children_count = 0;

    ast_T **temp = malloc(sizeof(struct AST_STRUCT *) * 4);
    int temp_size = 0;

    parser->token = lexer_get_next_token(parser->lexer);

    while (parser->token->type != TOKEN_REGION) {
        if (parser->token->type == TOKEN_ENDLINE) {
            if (temp_size != 0) {
                ast_T *new_variable_declaration = init_ast(AST_DEFINITION, NULL);
                new_variable_declaration->children = malloc(sizeof(struct AST_STRUCT *) * temp_size);
                new_variable_declaration->children_count = temp_size;
                memcpy(new_variable_declaration->children, temp, temp_size * sizeof(struct AST_STRUCT *));
                temp_size = 0;
                data_region_children_temp[data_region_children_count++] = new_variable_declaration;
            }
        } else {
            ast_T *child = init_ast(AST_ATOM, parser->token);
            temp[temp_size++] = child;
        }

        parser->token = lexer_get_next_token(parser->lexer);
    }

    if (strcmp(parser->token->value, "END") != 0) {
        printf("Region is not properly closed before initialization of another one!\n");
        exit(1);
    }

    data_region->children = malloc(sizeof(struct AST_STRUCT *) * data_region_children_count);
    data_region->children_count = data_region_children_count;
    memcpy(data_region->children, data_region_children_temp, data_region_children_count * sizeof(struct AST_STRUCT *));
    free(temp);
    free(data_region_children_temp);

    print_ast(data_region);

    return data_region;
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
            if ((strcmp(parser->token->value, "if") == 0) || (strcmp(parser->token->value, "else") == 0) || (strcmp(parser->token->value, "then") == 0))
                handle_conditional_function(parser);
            else
                handle_normal_function(parser);
            break;
        case TOKEN_REGION:
            switch (parser->token->value[0]) {
                case 'D':
                    handle_data_region(parser);
                    break;
            }
    }
}

*/

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

    free(token);
    return list;
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
            if (token_pop(depth)->value[0] != '{') 
                throw_error("Bad bracket");
            break;
        case ']':
            checked_token = token_pop(depth);
            if (token_pop(depth)->value[0] != '[') 
                throw_error("Bad bracket");
            break;
    }
    
    if (depth->size == 0) {
        free(token);
        free(checked_token);
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
    
    free(branching);
    return result;
}

token_T *expression_get_safe(list_T *tokens, int i) {
    if (tokens->top >= i) return NULL;

    return (token_T *)(tokens->array[i]);
}

ast_T **handle_funcall_parameters();

ast_T *parse_expression(list_T *tokens) {
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
            case TOKEN_IDENTIFIER:
                expression->subtype = EXPRESSION_IDENTIFIER;
                expression->params.literal_expression_params.token = token;
                if (expression_get_safe(expressions, i+1)->type == TOKEN_PARENTHESIS) {
                    token = tokens->array[++i];
                    if (token->value[0] != '(')
                        throw_token_error(token, "Expression error, expected '('.");

                    expression->subtype = EXPRESSION_FUNCALL;
                    expression->params.funcall_expression_params.function_expression = tokens->array[i - 1];
                    stack_T *arguments = init_stack();
                    depth = init_stack();
                    stack_push(depth, token);

                    list_T *expression_tokens = init_list(8);
                    int finished = 0;

                    while (!finished) {
                        if (++i >= tokens->top)
                            throw_error("Expression error, unfinished expression");
                        
                        token = tokens->array[++i];
                        switch (token->type) {
                            case TOKEN_PARENTHESIS: case TOKEN_CURLY_BRACE: case TOKEN_SQUARE_BRACKET:
                                if (handle_parenthesis_depth(depth, token)) {
                                    free(depth);
                                    if (expression_tokens->top > 0)
                                        stack_push(arguments, parse_expression(expression_tokens));
                                    finished = 1;
                                } else {
                                    list_push(expression_tokens, token);
                                }
                                break;
                            case TOKEN_DIVIDER:
                                free(token);
                                if (depth->size == 1) {
                                    // TODO: FIX POTENTIAL MEMORY LEAK
                                    stack_push(arguments, parse_expression(expression_tokens));
                                } else {
                                    list_push(expression_tokens, token);
                                }
                                break;
                            default:
                                list_push(expression_tokens, token);
                                break;
                        }
                    }
                    
                    expression->params.funcall_expression_params.param_expressions_size = arguments->size;
                    expression->params.funcall_expression_params.param_expressions = ast_stack_to_array(arguments, 0);
                    list_push(expressions, expression);
                } else if (expression_get_safe(tokens, i + 1)->type == TOKEN_SQUARE_BRACKET) {
                    token = tokens->array[++i];
                    if (token->value[0] != '[')
                        throw_token_error(token, "Expression error, expected '['.");

                    list_T *token_list = init_list(8);
                    depth = init_stack();
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

                                    free(depth);
                                    ast_T *indexing_expression = init_ast(EXPRESSION, EXPRESSION_INDEXING);
                                    indexing_expression->params.indexing_expression_params.arrray_expression = expression;
                                    indexing_expression->params.indexing_expression_params.index_expression = parse_expression(token_list);
                                    list_push(expressions, indexing_expression);
                                    finished = 1;
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
                    list_push(expressions, expression);
                    i++;
                }

                break;
            case TOKEN_PARENTHESIS:
                if (token->value[0] == ')')
                    throw_token_error(token, "Expression error, unexpected parenthesis detected.");

                free(token);
                expression->subtype = EXPRESSION_IN_PARENTHESIS;
                expression->params.parented_expression_params.expression = parse_expression(NULL);
                if (token->type != TOKEN_PARENTHESIS && token->value[0] != ')')
                    throw_token_error(token, "Expression error, expected ')'.");

                free(token);
            case TOKEN_UNARY_OPERATOR:
                printf("Unimplemented");
                exit(1);
                break;
            default:
                break;
        }
    }
    free(tokens);
    free(operators);
    ast_T *final_expression = list_pop(expressions);
    free(expressions->array);
    free(expressions);
    return final_expression;
}

ast_T *parse_statement(lexer_T *lexer, token_T *token) {
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
                    free(token);
                    statement->params.conditional_statement_params.condition_expression = parse_expression(get_tokens_inside_brackets(lexer));
                    statement->params.conditional_statement_params.if_block_statement = parse_statement(lexer, NULL);
                    statement->params.conditional_statement_params.else_block_statement = NULL;
                    token = lexer_peek(lexer);
                    if (token->type == TOKEN_KEYWORD && token->keyword_id == ELSE) {
                        lexer_get_next_token(lexer);
                        free(token);
                        statement->params.conditional_statement_params.else_block_statement = parse_statement(lexer, NULL);
                    }

                    break;
                case WHILE:
                    statement->subtype = STATEMENT_LOOP;
                    free(token);
                    statement->params.loop_statement_params.condition_expression = parse_expression(get_tokens_inside_brackets(lexer));
                    statement->params.loop_statement_params.statement = parse_statement(lexer, NULL);
                    break;
                case RETURN:
                    statement->subtype = STATEMENT_RETURN;
                    free(token);
                    statement->params.regular_expression_statement_params.expression = parse_expression(get_tokens_until_character(lexer, ';', NULL));
                    break;
                case LET:
                    free(statement);
                    statement = parse_variable_definition(lexer, DEFINITION_VARIABLE);
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
            free(token);
            token = lexer_get_next_token(lexer);

            while (token->type != TOKEN_CURLY_BRACE && token->value[0] != '}') {
                ast_push(stack, parse_statement(lexer, token));
                token = lexer_get_next_token(lexer);
            }

            statement->params.compound_statement_params.statement_count = stack->size;
            statement->params.compound_statement_params.statements = ast_stack_to_array(stack, 0);
            break;
        default:
            statement->subtype = STATEMENT_EXPRESSION;
            statement->params.regular_expression_statement_params.expression = parse_expression(get_tokens_until_character(lexer, ';', token));
            break;
    }

    return statement;
}

ast_T *parse_function_definition(lexer_T *lexer) {
    ast_T *ast = init_ast(DEFINITION, DEFINITION_FUNCTION);
    token_T *token = lexer_get_next_token(lexer);
    stack_T *stack = init_stack();

    if (!token_data_type(token))
        throw_token_error(token, "Function definition error, expected data type.");

    ast->params.function_definition_params.type = token;
    
    token = lexer_get_next_token(lexer);
    if (token->type != TOKEN_IDENTIFIER)
        throw_token_error(token, "Function definition error, expected identifier as name of function.");

    ast->params.function_definition_params.name = token;

    token = lexer_get_next_token(lexer);
    if (token->type != TOKEN_PARENTHESIS && token->value[0] != '(')
        throw_token_error(token, "Function definition error, expected '('.");
    
    free(token);

    // Get parameters
    token = lexer_get_next_token(lexer);
    if (token->value[0] != ')') {
        while (1) {
            if (!token_data_type(token))
                throw_token_error(token, "Function definition error, expected parameters or closing parenthesis.");

            token_push(stack, token);

            token = lexer_get_next_token(lexer);
            if (token->type != TOKEN_IDENTIFIER)
                throw_token_error(token, "Function definition error, expected identifier as parameter name.");

            token_push(stack, token);

            token = lexer_get_next_token(lexer);
            
            if(token->type == TOKEN_DIVIDER) {
                free(token);
                token = lexer_get_next_token(lexer);
            } else if (token->type == TOKEN_PARENTHESIS && token->value[0] == ')') {
                free(token);

                ast->params.function_definition_params.parameters_count = stack->size;
                ast->params.function_definition_params.parameters = token_stack_to_array(stack, 0);
                break;
            } else {
                throw_token_error(token, "Function definition error, expected ',' or ')'.");
            }
        }
    } else {
        free(token);
        ast->params.function_definition_params.parameters_count = 0;
        ast->params.function_definition_params.parameters = NULL;
    }

    ast->params.function_definition_params.statement = parse_statement(lexer, NULL);
    return ast;
}

ast_T *parse_variable_definition(lexer_T *lexer, int type) {
    ast_T *ast = init_ast(DEFINITION, type);

    token_T *token = lexer_get_next_token(lexer);
    if (!token_data_type(token))
        throw_token_error(token, "Variable definition error, expected data type.");

    ast->params.variable_definition_params.type = token;
    
    token = lexer_get_next_token(lexer);
    if (token->type != TOKEN_IDENTIFIER)
        throw_token_error(token, "Variable definition error, expected identifier as name of function.");

    ast->params.variable_definition_params.name = token;

    token = lexer_get_next_token(lexer);
    if (token->type != TOKEN_ASSIGNMENT)
        throw_token_error(token, "Variable definition error, expected '='.");

    free(token);
    ast->params.variable_definition_params.expression = parse_expression(get_tokens_until_character(lexer, ';', NULL));
    return ast;
}

ast_T *parse_uncertain(lexer_T *lexer) {
    token_T *token = lexer_get_next_token(lexer);

    switch (token->type) {
        case TOKEN_EOF:
            return NULL;
        case TOKEN_KEYWORD:
            switch(token->keyword_id) {
                case FUN:
                    free(token);
                    return parse_function_definition(lexer);
                case LET:
                    free(token);
                    return parse_variable_definition(lexer, DEFINITION_VARIABLE);
                case CONST:
                    free(token);
                    return parse_variable_definition(lexer, DEFINITION_CONSTANT);
                default:
                    throw_token_error(token, "Unexpected keyword. Expected LET, CONST or FUN.");
            }
    }

    throw_token_error(token, "Unexpected symbol, expected a keyword.");
    return NULL;
}

ast_T *parser_parse(parser_T *parser) {
    stack_T *programme = init_stack();
    while (1) {
        ast_T *new_ast = parse_uncertain(parser->lexer);
        if (new_ast == NULL)
            break;
        
        ast_push(programme, new_ast);
    }

    ast_T *programme_ast = init_ast(PROGRAMME, 0);
    programme_ast->params.programme_params.definitions_count = programme->size;
    programme_ast->params.programme_params.definitions = ast_stack_to_array(programme, 0);
    return programme_ast;
}
