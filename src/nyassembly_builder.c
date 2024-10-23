#include "include/nyassembly_builder.h"
#include "include/ast.h"
#include "include/data_types.h"
#include "include/defined_functions.h"
#include <stdio.h>
#include <string.h>

short swap_endian(short n) {
    int c = n >> 8;
    return ((n & 255) << 8) + c;
}

void print_command_binary(ast_T *ast, FILE *output) {
    fwrite(&(ast->command.code), sizeof(unsigned char), 1, output);
    if (ast->command.value_type == VALUE_INSIDE)
        fwrite(&(ast->command.value), sizeof(unsigned char), 1, output);
    else {
        unsigned int p = 0;
        fwrite(&p, sizeof(unsigned char), 1, output);
        p = swap_endian(ast->command.value);
        fwrite(&p, sizeof(unsigned short), 1, output);
    }
}

void print_retn(FILE *output, FILE *nyassembly_output) {
    unsigned char command_code = get_command_code("RETN");
    unsigned int p = 0;
    fprintf(nyassembly_output, "    %s, $%d\n", command_names[command_code], 0);
    fwrite(&command_code, sizeof(unsigned char), 1, output);
    fwrite(&p, sizeof(unsigned char), 1, output);
}

void print_command_readable(ast_T *ast, FILE *output) {
    fprintf(output, "    %s, $%d\n", command_names[ast->command.code], ast->command.value);
}

void build_variable(ast_T *variable, FILE *output, FILE *nyassembly_output) {
    struct VARIABLE_DEFINITION_PARAMS params = variable->params.variable_definition_params;
    fprintf(nyassembly_output, "%s %s, $%d\n", params.type->value, params.name->value, params.expression->command.value);
    fwrite(&(params.expression->command.value), data_type_sizes[params.expression->return_type], 1, output);
}

void build_nyassembly(ast_T *ast, FILE *output, FILE *nyassembly_output) {
    if (ast == NULL) return;

    switch(ast->type) {
        case PROGRAMME:
            fprintf(nyassembly_output, "section .DATA\n");
            for (int i = 0; i < ast->params.programme_params.variable_definitions_count; i++) {
                build_nyassembly(ast->params.programme_params.variable_definitions[i], output, nyassembly_output);
            }
            fprintf(nyassembly_output, "\n");
            fprintf(nyassembly_output, "section .CODE\n");
            for (int i = 0; i < ast->params.programme_params.function_definitions_count; i++) {
                build_nyassembly(ast->params.programme_params.function_definitions[i], output, nyassembly_output);
            }
            break;
        case DEFINITION:
            switch (ast->subtype) {
                case DEFINITION_FUNCTION:
                    fprintf(nyassembly_output, "%s: (%d)\n", ast->params.function_definition_params.name->value, ast->symbol->function->address);
                    fprintf(nyassembly_output, "    RAISE STACK BY: %d\n", ast->symbol->function->memory_assignment_end);
                    int p = swap_endian(ast->symbol->function->memory_assignment_end);
                    fwrite(&p, sizeof(unsigned short), 1, output);
                    for (int i = ast->params.function_definition_params.parameter_assignment_asts->top - 1; i >= 0 ; i--) {
                        print_command_binary(ast->params.function_definition_params.parameter_assignment_asts->array[i], output);
                        print_command_readable(ast->params.function_definition_params.parameter_assignment_asts->array[i], nyassembly_output);
                    }

                    build_nyassembly(ast->params.function_definition_params.statement, output, nyassembly_output);
                    if (!ast->is_returning)
                        print_retn(output, nyassembly_output);
                    break;
                case DEFINITION_VARIABLE:
                    build_variable(ast, output, nyassembly_output);
                    break;
            }
            break;
        case STATEMENT:
            switch (ast->subtype) {
                case STATEMENT_NOP:
                    break;
                case STATEMENT_LOOP:
                    print_command_binary(ast->params.loop_statement_params.statement, output);
                    print_command_readable(ast->params.loop_statement_params.statement, nyassembly_output);
                    build_nyassembly(ast->params.loop_statement_params.statement, output, nyassembly_output);
                    build_nyassembly(ast->params.loop_statement_params.condition_expression, output, nyassembly_output);
                    print_command_binary(ast, output);
                    print_command_readable(ast, nyassembly_output);
                    break;
                case STATEMENT_RETURN:
                    build_nyassembly(ast->params.regular_expression_statement_params.expression, output, nyassembly_output);
                    print_command_binary(ast, output);
                    print_command_readable(ast, nyassembly_output);
                    break;
                case STATEMENT_COMPOUND:
                    for (int i = 0; i < ast->params.compound_statement_params.statement_count; i++) {
                        build_nyassembly(ast->params.compound_statement_params.statements[i], output, nyassembly_output);
                    }
                    break;
                case STATEMENT_EXPRESSION:
                    build_nyassembly(ast->params.regular_expression_statement_params.expression, output, nyassembly_output);
                    break;
                case STATEMENT_CONDITIONAL:
                    build_nyassembly(ast->params.conditional_statement_params.condition_expression, output, nyassembly_output);
                    print_command_binary(ast, output);
                    print_command_readable(ast, nyassembly_output);
                    build_nyassembly(ast->params.conditional_statement_params.else_block_statement, output, nyassembly_output);
                    print_command_binary(ast->params.conditional_statement_params.else_block_statement, output);
                    print_command_readable(ast->params.conditional_statement_params.else_block_statement, nyassembly_output);
                    build_nyassembly(ast->params.conditional_statement_params.if_block_statement, output, nyassembly_output);
                    break;
                case STATEMENT_VARIABLE_DECLARATION:
                    build_nyassembly(ast->params.variable_definition_params.expression, output, nyassembly_output);
                    print_command_binary(ast, output);
                    print_command_readable(ast, nyassembly_output);
                    break;
            }
            break;
        case EXPRESSION:
            switch (ast->subtype) {
                case EXPRESSION_NUMBER: case EXPRESSION_IDENTIFIER:
                    print_command_binary(ast, output);
                    print_command_readable(ast, nyassembly_output);
                    break;
                case EXPRESSION_UNARY_OP:
                    build_nyassembly(ast->params.unary_op_expression_params.expression, output, nyassembly_output);
                    print_command_binary(ast, output);
                    print_command_readable(ast, nyassembly_output);
                    break;
                case EXPRESSION_BINARY_OP:
                    if (strcmp(ast->params.binary_op_expression_params.op->value, "=") != 0)
                        build_nyassembly(ast->params.binary_op_expression_params.l_expression, output, nyassembly_output);

                    build_nyassembly(ast->params.binary_op_expression_params.r_expression, output, nyassembly_output);
                    print_command_binary(ast, output);
                    print_command_readable(ast, nyassembly_output);
                    break;
                case EXPRESSION_FUNCALL:
                    for (int i = 0; i < ast->params.funcall_expression_params.param_expressions_size; i++)
                        build_nyassembly(ast->params.funcall_expression_params.param_expressions[i], output, nyassembly_output);
                    
                    print_command_binary(ast, output);
                    print_command_readable(ast, nyassembly_output);
                    break;
            }
        break;
    }   
}
