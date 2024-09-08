#include "include/ast.h"
#include "include/token.h"
#include <stdio.h>
#include <stdlib.h>

ast_T *init_ast(int type, int subtype) {
    ast_T *ast = malloc(sizeof(struct AST_STRUCT));
    ast->type = type;
    ast->subtype = subtype;

    return ast;
}

ast_T *ast_pop(stack_T *stack) {
    return (struct AST_STRUCT *)(stack_pop(stack));
}

ast_T *ast_peek(stack_T *stack) {
    return (struct AST_STRUCT *)(stack_peek(stack));
}

void ast_push(stack_T *stack, ast_T *ast) {
    stack_push(stack, ast);
}

ast_T **ast_stack_to_array(stack_T *stack, int keep_stack) {
    ast_T **asts = malloc(sizeof(struct AST_STRUCT *) * stack->size);

    for (int i = stack->size - 1; i >= 0; i--) {
        asts[i] = ast_pop(stack);
    }

    if (!keep_stack) free(stack);

    return asts;
}

void list_push_immediate_char(list_T *list, char c) {
    char *ch = malloc(sizeof(char));
    *ch = c;
    list_push(list, ch);
}

void list_pop_immediate(list_T *list) {
    free(list_pop(list));
}

void print_indentation(FILE *file, list_T *indentation) {
    fprintf(file, "  %c", *(char *)indentation->array[0] == 1 ? '|' : ' ');
    for (int i = 1; i < indentation->top; i++) {
        fprintf(file, "      %c", *(char *)indentation->array[i] == 1 ? '|' : ' ');
    }
}

void print_arrow(FILE *file, list_T *indentation) {
    char cached_value = *(char *)(indentation->array[indentation->top - 1]);
    *(char *)(indentation->array[indentation->top - 1]) = 1;
    print_indentation(file, indentation);
    *(char *)(indentation->array[indentation->top - 1]) = cached_value;
    fprintf(file, "==> ");
}

void print_ast(ast_T *ast, FILE *file, list_T *indentation) {
    switch(ast->type) {
        case PROGRAMME:
            indentation = init_list(8);
            fprintf(file, "PROGRAMME\n");
            for (int i = 0; i < ast->params.programme_params.definitions_count; i++) {
                list_push_immediate_char(indentation, (i != ast->params.programme_params.definitions_count - 1));
                print_ast(ast->params.programme_params.definitions[i], file, indentation);
                list_pop_immediate(indentation);
            }
            free(indentation);
            break;
        case DEFINITION:
            switch(ast->subtype) {
                case DEFINITION_FUNCTION:
                    print_arrow(file, indentation);
                    fprintf(file, "FUNCTION DEFINITION OUTPUT_TYPE=%s, NAME=%s\n", ast->params.function_definition_params.type->value, ast->params.function_definition_params.name->value);
                    list_push_immediate_char(indentation, 1);
                    print_arrow(file, indentation);
                    fprintf(file, "PARAMETERS=(");
                    int param_size = ast->params.function_definition_params.parameters_count / 2;
                    for (int i = 0; i < param_size; i++) {
                        fprintf(file, "%s %s", ast->params.function_definition_params.parameters[2*i]->value, ast->params.function_definition_params.parameters[2*i+1]->value);
                        if (i != param_size - 1)
                            fprintf(file, ", ");
                    }
                    fprintf(file, ")\n");
                    list_pop_immediate(indentation);
                    list_push_immediate_char(indentation, 0);
                    print_ast(ast->params.function_definition_params.statement, file, indentation);
                    list_pop_immediate(indentation);
                    break;
                case DEFINITION_VARIABLE:
                    print_arrow(file, indentation);
                    fprintf(file, "VARIABLE DEFINITION TYPE=%s NAME=%s\n", ast->params.variable_definition_params.type->value, ast->params.variable_definition_params.name->value);
                    list_push_immediate_char(indentation, 0);
                    print_ast(ast->params.variable_definition_params.expression, file, indentation);
                    list_pop_immediate(indentation);
                    break;
                case DEFINITION_CONSTANT:
                    print_arrow(file, indentation);
                    fprintf(file, "CONSTANT DEFINITION TYPE=%s NAME=%s\n", ast->params.variable_definition_params.type->value, ast->params.variable_definition_params.name->value);
                    list_push_immediate_char(indentation, 0);
                    print_ast(ast->params.variable_definition_params.expression, file, indentation);
                    list_pop_immediate(indentation);
                    break;
            }
            break;
        case EXPRESSION:
            print_arrow(file, indentation);
            fprintf(file, "EX: ");
            switch(ast->subtype) {
                case EXPRESSION_NUMBER: case EXPRESSION_IDENTIFIER:
                    fprintf(file, "%s\n", ast->params.literal_expression_params.token->value);
                    break;
                case EXPRESSION_INDEXING:
                    fprintf(file, "INDEXING: %s\n", ast->params.indexing_expression_params.arrray_expression->params.literal_expression_params.token->value);
                    list_push_immediate_char(indentation, 0);
                    print_ast(ast->params.indexing_expression_params.index_expression, file, indentation);
                    list_pop_immediate(indentation);
                    break;
                case EXPRESSION_FUNCALL:
                    fprintf(file, "FUNCALL: %s\n", ast->params.funcall_expression_params.function_expression->params.literal_expression_params.token->value);
                    for (int i = 0; i < ast->params.funcall_expression_params.param_expressions_size; i++) {
                        list_push_immediate_char(indentation, (i != ast->params.funcall_expression_params.param_expressions_size - 1));
                        print_ast(ast->params.funcall_expression_params.param_expressions[i], file, indentation);
                        list_pop_immediate(indentation);
                    }
                    break;
                default:
                    fprintf(file, "\n");
            }
            break;
        case STATEMENT:
            print_arrow(file, indentation);
            fprintf(file, "ST ");
            switch(ast->subtype) {
                case STATEMENT_COMPOUND:
                    fprintf(file, "COMP\n");
                    for (int i = 0; i < ast->params.compound_statement_params.statement_count; i++) {
                        list_push_immediate_char(indentation, i+1!=ast->params.compound_statement_params.statement_count);
                        print_ast(ast->params.compound_statement_params.statements[i], file, indentation);
                        list_pop_immediate(indentation);
                    }
                        
                    break;
                case STATEMENT_CONDITIONAL:
                    fprintf(file, "COND\n");
                    list_push_immediate_char(indentation, 1);
                    print_ast(ast->params.conditional_statement_params.condition_expression, file, indentation);
                    if (ast->params.conditional_statement_params.else_block_statement == NULL)
                        *(int *)(indentation->array[indentation->top-1]) = 0;
                    print_ast(ast->params.conditional_statement_params.if_block_statement, file, indentation);
                    *(int *)(indentation->array[indentation->top-1]) = 0;
                    if (ast->params.conditional_statement_params.else_block_statement != NULL)
                        print_ast(ast->params.conditional_statement_params.else_block_statement, file, indentation);
                    list_pop_immediate(indentation);
                    break;
                case STATEMENT_EXPRESSION:
                    fprintf(file, "EXPR\n");
                    list_push_immediate_char(indentation, 0);
                    print_ast(ast->params.regular_expression_statement_params.expression, file, indentation);
                    list_pop_immediate(indentation);
                    break;
                case STATEMENT_LOOP:
                    fprintf(file, "LOOP\n");
                    list_push_immediate_char(indentation, 1);
                    print_ast(ast->params.loop_statement_params.condition_expression, file, indentation);
                    *(int *)(indentation->array[indentation->top-1]) = 0;
                    print_ast(ast->params.loop_statement_params.statement, file, indentation);
                    list_pop_immediate(indentation);
                    break;
                case STATEMENT_NOP:
                    fprintf(file, "NOP\n");
                    break;
                case STATEMENT_RETURN:
                    fprintf(file, "RET\n");
                    list_push_immediate_char(indentation, 0);
                    print_ast(ast->params.regular_expression_statement_params.expression, file, indentation);
                    list_pop_immediate(indentation);
                    break;
                case STATEMENT_VARIABLE_DECLARATION:
                    fprintf(file, "DEFVAR TYPE=%s NAME=%s\n", ast->params.variable_definition_params.type->value, ast->params.variable_definition_params.name->value);
                    list_push_immediate_char(indentation, 0);
                    print_ast(ast->params.variable_definition_params.expression, file, indentation);
                    list_pop_immediate(indentation);
                    break;
            }
            break;
    }
}

/*
void print_ast(ast_T *ast) {
    char *readable_type;
    switch (ast->type) {
        case AST_ATOM:
            readable_type = "ATOM";
            break;
        case AST_COMPOUND:
            readable_type = "COMPOUND";
            break;
        case AST_START:
            readable_type = "START";
            break;
        case AST_CONDITIONAL:
            readable_type = "CONDITIONAL";
            break;
        case AST_DEFINITION:
            readable_type = "DEFINITION";
            break;
        case AST_REGION:
            readable_type = "REGION";
            break;
    }

    printf("(AST %s", readable_type);
    if (ast->token != NULL) {
        printf(" ");
        print_token(ast->token);
    }
    if (ast->children_count > 0) {
        printf(" (CHILDREN ");
        for (int i = 0; i < ast->children_count; i++) {
            print_ast(ast->children[i]);
            printf(" ");
        }
        printf(")");
    }
    printf(")");
}
*/
