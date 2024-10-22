#include "include/semantic_analyzer.h"
#include "include/ast.h"
#include "include/data_types.h"
#include "include/defined_functions.h"
#include "include/symbols.h"
#include "stdlib.h"
#include "include/error.h"
#include "string.h"
#include <stdio.h>

void analyze_expression(semantic_analyzer_T *analyzer, ast_T *expression);
void analyze_statement(semantic_analyzer_T *analyzer, ast_T *statement);
void analyze_variable(semantic_analyzer_T *analyzer, ast_T *variable);

// Returns data type of expression
void analyze_number(semantic_analyzer_T *analyzer, ast_T *expression) {
    int converted_number = atoi(expression->params.literal_expression_params.token->value);
    if (converted_number < 256) {
        expression->size = 1;
        expression->command.value = converted_number;
        expression->command.value_type = VALUE_INSIDE;
        expression->command.code = get_command_code("LNUM");
        expression->return_type = SINT;
    } else if (converted_number < 65536) {
        expression->size = 2;
        expression->command.value = converted_number;
        expression->command.value_type = VALUE_OUTSIDE;
        expression->command.code = get_command_code("LNUML");
        expression->return_type = UBYTE;
    } else {
        throw_error("Numbers above 65535 are not supported yet!");
    }
}

void analyze_identifier(semantic_analyzer_T *analyzer, ast_T *expression) {
    symbol_T *sym = expression->symbol;
    char command[10] = "";
    switch (sym->type) {
        case variable:
            expression->return_type = sym->variable->type;
            strcat(command, "LDR");
            if (sym->variable->global == 0)
                strcat(command, "L");
            switch(sym->variable->type) {
                case SINT:
                    strcat(command, "S");
                    break;
                case SBYTE:
                    strcat(command, "SB");
                    break;
                case UBYTE:
                    strcat(command, "B");
                    break;
                case SSHORT:
                    strcat(command, "SH");
                    break;
                case USHORT:
                    strcat(command, "H");
                    break;
            }
            unsigned int memory_address = sym->variable->global ? sym->variable->address :
                                                                  sym->variable->context->memory_assignment_end - sym->variable->offset;
            if (memory_address >= 256) {
                strcat(command, "L");
                expression->command.value_type = VALUE_OUTSIDE;
                expression->size = 2;
            } else {
                expression->size = 1;
                expression->command.value_type = VALUE_INSIDE;
            }
            expression->command.code = get_command_code(command);
            expression->command.value = memory_address;
            break;
        case constant:
            // TODO: ADD AST_FREE METHOD AND FREE OLD EXPRESSION NOT TO LEAK MEMORY!
            expression->subtype = ((ast_T *)(sym->constant->expression))->subtype;
            expression->size = ((ast_T *)(sym->constant->expression))->size;
            expression->return_type = ((ast_T *)(sym->constant->expression))->return_type;
            expression->params = ((ast_T *)(sym->constant->expression))->params;
            expression->command = ((ast_T *)(sym->constant->expression))->command;
            analyze_expression(analyzer, expression);
            break;
        case function:
            throw_token_error(expression->params.literal_expression_params.token, "Using a function without calling it is not supported.");
    }
}

void analyze_binary_operator(semantic_analyzer_T *analyzer, ast_T *expression) {
    struct BINARY_OP_EXPRESSION_PARAMS params = expression->params.binary_op_expression_params;
    analyze_expression(analyzer, params.l_expression);
    analyze_expression(analyzer, params.r_expression);
    expression->size = params.l_expression->size + params.r_expression->size + 1;
    expression->command.value = 0;
    expression->command.value_type = VALUE_INSIDE;
    handle_binary_operator_expression(expression);
}

void analyze_unary_operator(semantic_analyzer_T *analyzer, ast_T *expression) {
    struct UNARY_OP_EXPRESSION_PARAMS params = expression->params.unary_op_expression_params;
    analyze_expression(analyzer, params.expression);
    expression->size = params.expression->size + 1;
    expression->command.value = 0;
    expression->command.value_type = VALUE_INSIDE;
    handle_unary_operator_expression(expression);
}

void analyze_funcall(semantic_analyzer_T *analyzer, ast_T *expression) {
    struct FUNCALL_EXPRESSION_PARAMS params = expression->params.funcall_expression_params;
    function_T *function = expression->symbol->function;
    for (int i = 0; i < params.param_expressions_size; i++) {
        ast_T *child_expression = params.param_expressions[i];
        analyze_expression(analyzer, child_expression);
        expression->size += child_expression->size;

        if (data_type_is_number(function->param_datatypes[i])) {
            if (!data_type_is_number(child_expression->return_type)) {
                printf("%d. parameter of function is not a number. Got: %s\n", i, data_type_strings[params.param_expressions[i]->return_type]);
                exit(1);
            }
        } else if (function->param_datatypes[i] != child_expression->return_type) {
                printf("%d. parameter of function expected to be: %s. Got: %s\n", i, data_type_strings[function->param_datatypes[i]], data_type_strings[params.param_expressions[i]->return_type]);
                exit(1);
        }
    }

    expression->return_type = function->return_type;
    expression->command.code = get_command_code("CALL");
    expression->command.value_type = VALUE_OUTSIDE;
    expression->command.value = function->address;
    expression->size += 2;
}

void analyze_expression(semantic_analyzer_T *analyzer, ast_T *expression) {
    switch(expression->subtype) {
        case EXPRESSION_NUMBER:
            analyze_number(analyzer, expression);
            break;
        case EXPRESSION_FUNCALL:
            analyze_funcall(analyzer, expression);
            break;
        case EXPRESSION_INDEXING:
            printf("NOT IMPLEMENTED");
            exit(1);
            break;
        case EXPRESSION_UNARY_OP:
            analyze_unary_operator(analyzer, expression);
            break;
        case EXPRESSION_BINARY_OP:
            analyze_binary_operator(analyzer, expression);
            break;
        case EXPRESSION_IDENTIFIER:
            analyze_identifier(analyzer, expression);
            break;
    }
}

void analyze_loop_statement(semantic_analyzer_T *analyzer, ast_T *statement) {
    struct LOOP_STATEMENT_PARAMS params = statement->params.loop_statement_params;
    analyze_expression(analyzer, params.condition_expression);
    analyze_statement(analyzer, params.statement);

    // Add a forward jump to condition
    if (params.statement->size < 255) {
        params.statement->command.code = get_command_code("JMP");
        params.statement->command.value_type = VALUE_INSIDE;
        statement->size = 1;
    }  else {
        params.statement->command.code = get_command_code("JMPL");
        params.statement->command.value_type = VALUE_OUTSIDE;
        statement->size = 2;
    }
    params.statement->command.value = params.statement->size;

    // Add conditioned backwards jump
    unsigned int cached_size = params.condition_expression->size + params.statement->size;
    if (cached_size < 254) {
        statement->size += 1;
        statement->command.code = get_command_code("CJMPB");
        statement->command.value_type = VALUE_INSIDE;
    } else {
        statement->size += 2;
        statement->command.code = get_command_code("CJMPBL");
        statement->command.value_type = VALUE_OUTSIDE;
    }

    statement->size += cached_size;
    statement->command.value = cached_size;
}

void analyze_return_statement(semantic_analyzer_T *analyzer, ast_T *statement) {
    ast_T *expression = statement->params.regular_expression_statement_params.expression;
    unsigned char return_type = NONE;
    if (expression != NULL) {
        analyze_expression(analyzer, expression);
        return_type = expression->return_type;
    }
    function_T *function = get_symbol(analyzer->context, NULL)->function;
    if (data_type_is_number(function->return_type)) {
        if (!data_type_is_number(return_type)) {
            printf("Trying to return incorrect data type from function %s. Expected: a number, got: %s.\n", analyzer->context, data_type_strings[return_type]);
            exit(1);
        }
    } else if (function->return_type != return_type) {
        printf("Trying to return incorrect data type from function %s. Expected: %s, got: %s.\n", analyzer->context, data_type_strings[function->return_type], data_type_strings[return_type]);
            exit(1);
    }

    statement->size = return_type == NONE ? 1 : expression->size + 1;
    statement->command.value_type = VALUE_INSIDE;
    statement->command.value = 0;
    statement->command.code = return_type == NONE ? get_command_code("RETN") : get_command_code("RET");
    statement->is_returning = 1;
}

void analyze_conditional_statement(semantic_analyzer_T *analyzer, ast_T *statement) {
    struct CONDITIONAL_STATEMENT_PARAMS params = statement->params.conditional_statement_params;
    analyze_expression(analyzer, params.condition_expression);
    analyze_statement(analyzer, params.if_block_statement);

    // Create else block if it doesnt exist already
    if (params.else_block_statement == NULL)
        params.else_block_statement = init_ast(STATEMENT, STATEMENT_NOP);

    // Analyze else block + assign correct command
    analyze_statement(analyzer, params.else_block_statement);
    if (params.if_block_statement->size < 256) {
        params.else_block_statement->command.code = get_command_code("JMP");
        params.else_block_statement->command.value_type = VALUE_INSIDE;
        params.else_block_statement->size += 1;
    } else {
        params.else_block_statement->command.code = get_command_code("JMPL");
        params.else_block_statement->command.value_type = VALUE_OUTSIDE;
        params.else_block_statement->size += 2;
    }
    params.else_block_statement->command.value = params.if_block_statement->size;

    // Assign correct command to if block
    statement->size = params.if_block_statement->size + params.condition_expression->size + params.else_block_statement->size;
    if (params.else_block_statement->size < 256) {
        statement->command.code = get_command_code("CJMP");
        statement->command.value_type = VALUE_INSIDE;
        statement->size += 1;
    } else {
        statement->command.code = get_command_code("CJMPL");
        statement->command.value_type = VALUE_OUTSIDE;
        statement->size += 2;
    }
    statement->params.conditional_statement_params.else_block_statement = params.else_block_statement;
    statement->command.value = params.else_block_statement->size;
}

void analyze_compound_statement(semantic_analyzer_T *analyzer, ast_T *statement) {
    int last_statement_index = -1;
    for (int i = 0; i < statement->params.compound_statement_params.statement_count; i++) {
        analyze_statement(analyzer, statement->params.compound_statement_params.statements[i]);
        statement->size += statement->params.compound_statement_params.statements[i]->size;
        last_statement_index = i;
    };
    if (last_statement_index >= 0 && statement->params.compound_statement_params.statements[last_statement_index]->subtype == STATEMENT_RETURN)
        statement->is_returning = 1;
}

void analyze_variable_declaration_statement(semantic_analyzer_T *analyzer, ast_T *statement) {
    analyze_variable(analyzer, statement);
    statement->symbol = get_symbol(statement->params.variable_definition_params.name->value, analyzer->context);
    handle_variable_assignment(statement);
}

void analyze_statement(semantic_analyzer_T *analyzer, ast_T *statement) {
    switch (statement->subtype) {
        case STATEMENT_COMPOUND:
            analyze_compound_statement(analyzer, statement);
            break;
        case STATEMENT_NOP:
            statement->size = 0;
            break;
        case STATEMENT_LOOP:
            analyze_loop_statement(analyzer, statement);
            break;
        case STATEMENT_RETURN:
            analyze_return_statement(analyzer, statement);
            break;
        case STATEMENT_EXPRESSION:
            analyze_expression(analyzer, statement->params.regular_expression_statement_params.expression);
            statement->size = statement->params.regular_expression_statement_params.expression->size;
            break;
        case STATEMENT_CONDITIONAL:
            analyze_conditional_statement(analyzer, statement);
            break;
        case STATEMENT_VARIABLE_DECLARATION:
            analyze_variable_declaration_statement(analyzer, statement);
            break;
    }
}

void analyze_function(semantic_analyzer_T *analyzer, ast_T *function) {
    struct FUNCTION_DEFINITION_PARAMS params = function->params.function_definition_params;
    analyzer->context = params.name->value;
    function_T *function_symbol = function->symbol->function;
    function_symbol->address = analyzer->defined_function_sizes;

    analyze_statement(analyzer, params.statement);
    function->size = params.statement->size + 1;
    if (!params.statement->is_returning) {
        if (function_symbol->return_type != NONE) {
            printf("Function %s does not return a value in every possible case", params.name->value);
            exit(1);
        }

        function->size += 1;
        function->is_returning = 0;
    } else {
        function->is_returning = 1;
    }

    analyzer->defined_function_sizes += function->size;
    analyzer->context = NULL;
}

void analyze_variable(semantic_analyzer_T *analyzer, ast_T *variable) {
    struct VARIABLE_DEFINITION_PARAMS *params = &variable->params.variable_definition_params;
    variable_T *var = variable->symbol->variable;
    analyze_expression(analyzer, params->expression);

    if (data_type_is_number(var->type)) {
        if (!data_type_is_number(params->expression->return_type)) {
            printf("Expression inside variable %s is not a number. Got: %s\n", params->name->value, data_type_strings[params->expression->return_type]);
            exit(1);
        }
    } else if (var->type != params->expression->return_type) {
            printf("Expression inside variable %s expected to be: %s. Got: %s\n", params->name->value, data_type_strings[var->type], data_type_strings[params->expression->return_type]);
            exit(1);
    }
}

void analyze_constant(semantic_analyzer_T *analyzer, ast_T *constant) {
    return;
}

semantic_analyzer_T *init_semantic_analyzer() {
    return (semantic_analyzer_T *)calloc(1, sizeof(struct SEMANTIC_ANALYZER_STRUCT));
}

ast_T *semantic_analyze(semantic_analyzer_T *analyzer, ast_T *programme) {
    for (int i = 0; i < programme->params.programme_params.variable_definitions_count; i++) {
        ast_T *definition = programme->params.programme_params.variable_definitions[i];
        
        definition->subtype == DEFINITION_CONSTANT ? analyze_constant(analyzer, definition) : analyze_variable(analyzer, definition);
    }

    for (int i = 0; i < programme->params.programme_params.function_definitions_count; i++) {
        ast_T *definition = programme->params.programme_params.function_definitions[i];
        
        analyze_function(analyzer, definition);
    }
   
    return programme;
}
