#include "include/optimalizer.h"
#include "include/ast.h"
#include "include/defined_functions.h"
#include "include/semantic_analyzer.h"
#include <stdio.h>

void optimalize(ast_T *ast) {
    switch (ast->type) {
        case PROGRAMME:
            break;
        case DEFINITION:
            break;
        case STATEMENT:
            break;
        case EXPRESSION:
            switch (ast->subtype) {
                case EXPRESSION_BINARY_OP:
                    if (ast->params.binary_op_expression_params.r_expression->subtype == EXPRESSION_NUMBER) {
                        ast_T *l_expression = ast->params.binary_op_expression_params.l_expression;
                        ast_T *r_expression = ast->params.binary_op_expression_params.r_expression;

                        if (ast->params.binary_op_expression_params.l_expression->subtype == EXPRESSION_NUMBER) {
                            ast->subtype = EXPRESSION_NUMBER;
                            struct BINARY_OP_EXPRESSION_PARAMS *params = &ast->params.binary_op_expression_params;
                            char strbuf[32] = "";
                            int res = 0;
                            switch(string_to_int(ast->params.binary_op_expression_params.op->value)) {
                                case '+':
                                    res = l_expression->command.value + r_expression->command.value;
                                    break;
                                case '-':
                                    res = l_expression->command.value - r_expression->command.value;
                                    break;
                                case '*':
                                    res = l_expression->command.value * r_expression->command.value;
                                    break;
                                case '/':
                                    res = l_expression->command.value / r_expression->command.value;
                                    break;
                                case '%':
                                    res = l_expression->command.value % r_expression->command.value;
                                    break;
                                    
                            }
                            sprintf(strbuf, "%d", res);
                            token_T *new_token = init_token(TOKEN_NUMBER, strbuf, params->op->line, params->op->char_index, 0); 
                            free_token(params->op);
                            free_ast(l_expression, 1);
                            free_ast(r_expression, 1);
                            ast->params.literal_expression_params.token = new_token;
                            analyze_number(ast);
                        }
                    }
                    break;
            }
            break;
    }
}

