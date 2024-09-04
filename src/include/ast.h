#ifndef AST_H
#define AST_H

typedef struct AST_STRUCT ast_T;

#include "token.h"
#include "stack.h"
#include "defined_functions.h"
#include "list.h"
#include <stdio.h>
/*
struct AST_STRUCT {
    enum {
        AST_START,
        AST_COMPOUND,
        AST_DEFINITION,
        AST_REGION,
        AST_ATOM,
        AST_CONDITIONAL
    } type;
    unsigned int size_bytes;
    unsigned int children_count;
    unsigned short additional_data;
    enum {
        ADDITIONAL_DATA_BEHIND = 0,
        ADDITIONAL_DATA_INSIDE = 1
    } additional_data_position;
    unsigned int output_count;
    token_T *token;
    ast_T **children;
    defined_function_T *assigned_defined_function;
    unsigned int selected_defined_function_variation;
};

*/

enum {
    PROGRAMME,
    DEFINITION,
    STATEMENT,
    EXPRESSION
} AST_TYPES;

enum {
    DEFINITION_FUNCTION,
    DEFINITION_CONSTANT,
    DEFINITION_VARIABLE,
} AST_DEFINITION_SUBTYPES;

enum {
    STATEMENT_COMPOUND,
    STATEMENT_CONDITIONAL,
    STATEMENT_LOOP,
    STATEMENT_RETURN,
    STATEMENT_VARIABLE_DECLARATION,
    STATEMENT_EXPRESSION,
    STATEMENT_NOP
} AST_STATEMENT_SUBTYPES;

enum {
    EXPRESSION_NUMBER,
    EXPRESSION_IDENTIFIER,
    EXPRESSION_IN_PARENTHESIS,
    EXPRESSION_INDEXING,
    EXPRESSION_FUNCALL,
    EXPRESSION_BINARY_OP,
    EXPRESSION_UNARY_OP
} AST_EXPRESSION_SUBTYPES;

struct PROGRAMME_PARAMS {
    ast_T **definitions;
    int definitions_count;
};

struct VARIABLE_DEFINITION_PARAMS {
    token_T *type, *name;
    ast_T *expression;
};

struct FUNCTION_DEFINITION_PARAMS {
    token_T *type, *name;
    token_T **parameters;
    int parameters_count;
    ast_T *statement;
};

struct COMPOUND_STATEMENT_PARAMS {
    ast_T **statements;
    int statement_count;
};

struct CONDITIONAL_STATEMENT_PARAMS {
    ast_T *condition_expression, *if_block_statement, *else_block_statement;
};

struct LOOP_STATEMENT_PARAMS {
    ast_T *condition_expression, *statement;
};

struct REGULAR_EXPRESSION_STATEMENT_PARAMS {
    ast_T *expression;
};

struct LITERAL_EXPRESSION_PARAMS {
    token_T *token;
};

struct PARENTED_EXPRESSION_PARAMS {
    ast_T *expression;
};

struct INDEXING_EXPRESSION_PARAMS {
    ast_T *arrray_expression, *index_expression;
};

struct FUNCALL_EXPRESSION_PARAMS {
    ast_T *function_expression;
    ast_T **param_expressions;
    unsigned int param_expressions_size;
};

struct BINARY_OP_EXPRESSION_PARAMS {
    ast_T *l_expression, *r_expression;
    token_T *operator;
};

struct UNARY_OP_EXPRESSION_PARAMS {
    ast_T *expression;
    token_T *operator;
};

struct AST_STRUCT {
    int type;
    int subtype;
    union {
        struct PROGRAMME_PARAMS programme_params;
        struct VARIABLE_DEFINITION_PARAMS variable_definition_params;
        struct FUNCTION_DEFINITION_PARAMS function_definition_params;
        struct COMPOUND_STATEMENT_PARAMS compound_statement_params;
        struct CONDITIONAL_STATEMENT_PARAMS conditional_statement_params;
        struct LOOP_STATEMENT_PARAMS loop_statement_params;
        struct REGULAR_EXPRESSION_STATEMENT_PARAMS regular_expression_statement_params;
        struct LITERAL_EXPRESSION_PARAMS literal_expression_params;
        struct PARENTED_EXPRESSION_PARAMS parented_expression_params; 
        struct INDEXING_EXPRESSION_PARAMS indexing_expression_params;
        struct FUNCALL_EXPRESSION_PARAMS funcall_expression_params;
        struct BINARY_OP_EXPRESSION_PARAMS binary_op_expression_params;
        struct UNARY_OP_EXPRESSION_PARAMS unary_op_expression_params;
    } params;
};

ast_T *init_ast(int type, int subtype);

ast_T *ast_pop(stack_T *stack);

ast_T *ast_peek(stack_T *stack);

void ast_push(stack_T *stack, ast_T *ast);

ast_T **ast_stack_to_array(stack_T *stack, int keep_stack);

void print_ast(ast_T *ast, FILE *file, list_T *indentation);

#endif