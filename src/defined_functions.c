#include "include/defined_functions.h"
#include "include/error.h"
#include "include/hashmap.h"
#include "include/data_types.h"
#include "include/symbols.h"
#include "include/lexer.h"
#include "include/token.h"
#include "stdio.h"
#include "include/ast.h"
#include <stdio.h>
#include <string.h>

#define ARR_LEN(X) (sizeof(X) / sizeof(X[0]))

char command_names[][10] = { "STOP", "LNUM", "LNUML", "LDR", "LDRL", "LDRH",
                             "LDRHL", "LDRB", "LDRBL", "LDRL", "LDRLL", "LDRLH",
                             "LDRLHL", "LDRLB", "LDRLBL", "LDRS", "LDRSL", "LDRSH",
                             "LDRSHL", "LDRSB", "LDRSBL", "LDRLS", "LDRLSL", "LDRLSH",
                             "LDRLSHL", "LDRLSB", "LDRLSBL", "STR", "STRL", "STRH",
                             "STRHL", "STRB", "STRBL", "STRL", "STRLL", "STRLH",
                             "STRLHL", "STRLB", "STRLBL", "STRS", "STRSL", "STRSH",
                             "STRSHL", "STRSB", "STRSBL", "STRLS", "STRLSL", "STRLSH",
                             "STRLSHL", "STRLSB", "STRLSBL", "ADD", "SUB", "MUL",
                             "DIV", "MOD", "BSL", "BSR", "CMPL", "CMPG",
                             "CMPLEQ", "CMPGEQ", "CMPEQ", "CMPNEQ", "BAND", "BOR",
                             "BXOR", "AND", "OR", "NOT", "CALL", "RET",
                             "RETN", "JMP", "JMPL", "CJMP", "CJMPL", "JMPB",
                             "JMPBL", "CJMPB", "CJMPBL", "DROP", "PRINT" };

const unsigned char number_datatypes[] = { SINT, UINT, SBYTE, UBYTE, SSHORT, USHORT };
hashmap_T *command_codes = NULL;

void generate_command_codes() {
    command_codes = new_hashmap(512);
    for (int i = 0; i < ARR_LEN(command_names); i++) {
        unsigned char *code = malloc(sizeof(unsigned char));
        *code = i;
        hashmap_set(command_codes, command_names[i], code);
    }
}

void init_defined_functions() {
    generate_command_codes();
    FILE *defined_functions_config = fopen("src/config/defined_functions.cfg", "r");
    lexer_T *lexer = init_lexer(defined_functions_config, "config/defined_functions.cfg");
    token_T *token = lexer_get_next_token(lexer);
    while(token->type != TOKEN_EOF) {
        if (token->type != TOKEN_KEYWORD || token->keyword_id != FUN)
            throw_token_error(token, "Expected FUN.");
        free_token(token);
        token = lexer_get_next_token(lexer);
        if (token->type != TOKEN_DATA_TYPE)
            throw_token_error(token, "Expected datatype.");
        unsigned char return_type = token->data_type_id;
        free_token(token);
        token_T *name_token = lexer_get_next_token(lexer);
        if (name_token->type != TOKEN_IDENTIFIER)
            throw_token_error(name_token, "Expected function name.");
        token = lexer_get_next_token(lexer);
        if (token->type != TOKEN_PARENTHESIS || token->value[0] != '(')
            throw_token_error(token, "Expected opening parenthesis.");
        free_token(token);
        list_T *parameter_datatypes = init_list(8);
        token = lexer_peek(lexer);
        if (token->type == TOKEN_PARENTHESIS) {
            if (token->value[0] != ')')
                throw_token_error(token, "Expected closing parenthesis.");

            free_token(token);
        } else {
            while (1) {
                token = lexer_get_next_token(lexer);
                if (token->type != TOKEN_DATA_TYPE)
                    throw_token_error(token, "Expected datatype");

                list_push(parameter_datatypes, token);
                
                token = lexer_get_next_token(lexer);
                if (token->type == TOKEN_PARENTHESIS) {
                    if (token->value[0] != ')')
                        throw_token_error(token, "Expected closing parenthesis.");

                    free_token(token);
                    break;
                } else if (token->type == TOKEN_DIVIDER) {
                    free_token(token);
                } else {
                    throw_token_error(token, "Expected closing parenthesis or ',' to indicate new parameter");
                }
            }
        }
        token = lexer_get_next_token(lexer);
        if (token->type != TOKEN_SEMICOLON)
            throw_token_error(token, "';' expected.");
        free_token(token);

        init_embedded_function(name_token->value, return_type, 0, parameter_datatypes);
        free_token(name_token);
        token = lexer_get_next_token(lexer);
    }

    free_token(token);
    free_lexer(lexer);
    fclose(defined_functions_config);
}

void free_command_codes() {
    free_hashmap(command_codes, 1);
}

unsigned char get_command_code(char *name) {
    unsigned char *res = hashmap_get(command_codes, name);
    if (res == NULL) {
        printf("HYPER ERROR! COMMAND: %s NOT FOUND IN DATABASE!\n", name);
        exit(1);
    }

    return *res;
}

unsigned int string_to_int(char *str) {
    int res = 0;

    for (int i = 0;;i++) {
        if (str[i] == '\0')
            break;

        res += str[i] << (i * 8);
    }
    
    return res;
}

unsigned char data_type_is_number(unsigned char data_type) {
    for (int i = 0; i < ARR_LEN(number_datatypes); i++) {
        if (number_datatypes[i] == data_type)
            return 1;
    }

    return 0;
}

void throw_error_unless_correct_datatypes(int res) {
    if (res) return;

    printf("Operator doesnt have correct children datatypes");
    exit(1);
}

void handle_unary_operator_expression(void *ast) {
    ast_T *expression = (ast_T *)ast;
    struct UNARY_OP_EXPRESSION_PARAMS params = expression->params.unary_op_expression_params;
    switch (string_to_int(expression->params.unary_op_expression_params.op->value)) {
        case '!':
            throw_error_unless_correct_datatypes(data_type_is_number(params.expression->return_type));
            expression->return_type = SBYTE;
            expression->command.code = get_command_code("NOT");
            break;
        case ':d':
            throw_error_unless_correct_datatypes(params.expression->return_type != NONE);
            expression->return_type = NONE;
            expression->command.code = get_command_code("DROP");
            break;
    }
}

void handle_variable_assignment(void *ast) {
    ast_T *expression = (ast_T *)ast;
    ast_T *l_expression = NULL;
    ast_T *r_expression = NULL;
    if (expression->type == STATEMENT) {
        l_expression = init_ast(EXPRESSION, EXPRESSION_IDENTIFIER, expression->line, expression->char_index);
        l_expression->symbol = expression->symbol;
        r_expression = expression->params.variable_definition_params.expression;
    } else {
        l_expression = expression->params.binary_op_expression_params.l_expression;
        r_expression = expression->params.binary_op_expression_params.r_expression;
    }
    if (l_expression->subtype != EXPRESSION_IDENTIFIER && l_expression->symbol->type != variable)
        throw_error("Can only assign values to a variable");

    variable_T *var = l_expression->symbol->variable;
    if (expression->type == STATEMENT)
        free(l_expression);
    if (data_type_is_number(var->type))
        throw_error_unless_correct_datatypes(data_type_is_number(var->type) &&
                                             data_type_is_number(r_expression->return_type));
    else
        (throw_error_unless_correct_datatypes(var->type != r_expression->return_type));
    char command[10] = "";
    strcat(command, "STR");
    if (var->global == 0)
        strcat(command, "L");
    switch(var->type) {
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
    unsigned int memory_address = (var->global ? var->address : var->context->memory_assignment_end - var->offset);
    if (memory_address >= 256) {
        strcat(command, "L");
        expression->command.value_type = VALUE_OUTSIDE;
        expression->size = 2 + r_expression->size;
    } else {
        expression->size = 1 + r_expression->size;
        expression->command.value_type = VALUE_INSIDE;
    }
    expression->command.code = get_command_code(command);
    expression->command.value = memory_address;
}

void handle_binary_operator_expression(void *ast) {
    ast_T *expression = (ast_T *)ast;
    struct BINARY_OP_EXPRESSION_PARAMS params = expression->params.binary_op_expression_params;
    switch (string_to_int(expression->params.binary_op_expression_params.op->value)) {
        case '+':
            throw_error_unless_correct_datatypes(data_type_is_number(params.l_expression->return_type) &&
                                                 data_type_is_number(params.r_expression->return_type));
            expression->return_type = SINT;
            expression->command.code = get_command_code("ADD");
            break;
        case '-':
            throw_error_unless_correct_datatypes(data_type_is_number(params.l_expression->return_type) &&
                                                 data_type_is_number(params.r_expression->return_type));
            expression->return_type = SINT;
            expression->command.code = get_command_code("SUB");
            break;
        case '*':
            throw_error_unless_correct_datatypes(data_type_is_number(params.l_expression->return_type) &&
                                                 data_type_is_number(params.r_expression->return_type));
            expression->return_type = SINT;
            expression->command.code = get_command_code("MUL");
            break;
        case '/':
            throw_error_unless_correct_datatypes(data_type_is_number(params.l_expression->return_type) &&
                                                 data_type_is_number(params.r_expression->return_type));
            expression->return_type = SINT;
            expression->command.code = get_command_code("DIV");
            break;
        case '%':
            throw_error_unless_correct_datatypes(data_type_is_number(params.l_expression->return_type) &&
                                                 data_type_is_number(params.r_expression->return_type));
            expression->return_type = SINT;
            expression->command.code = get_command_code("MOD");
            break;
        case '<<':
            throw_error_unless_correct_datatypes(data_type_is_number(params.l_expression->return_type) &&
                                                 data_type_is_number(params.r_expression->return_type));
            expression->return_type = SINT;
            expression->command.code = get_command_code("BSL");
            break;
        case '>>':
            throw_error_unless_correct_datatypes(data_type_is_number(params.l_expression->return_type) &&
                                                 data_type_is_number(params.r_expression->return_type));
            expression->return_type = SINT;
            expression->command.code = get_command_code("BSR");
            break;
        case '<':
            throw_error_unless_correct_datatypes(data_type_is_number(params.l_expression->return_type) &&
                                                 data_type_is_number(params.r_expression->return_type));
            expression->return_type = SBYTE;
            expression->command.code = get_command_code("CMPL");
            break;
        case '>':
            throw_error_unless_correct_datatypes(data_type_is_number(params.l_expression->return_type) &&
                                                 data_type_is_number(params.r_expression->return_type));
            expression->return_type = SBYTE;
            expression->command.code = get_command_code("CMPG");
            break;
        case '<=':
            throw_error_unless_correct_datatypes(data_type_is_number(params.l_expression->return_type) &&
                                                 data_type_is_number(params.r_expression->return_type));
            expression->return_type = SBYTE;
            expression->command.code = get_command_code("CMPLEQ");
            break;
        case '>=':
            throw_error_unless_correct_datatypes(data_type_is_number(params.l_expression->return_type) &&
                                                 data_type_is_number(params.r_expression->return_type));
            expression->return_type = SBYTE;
            expression->command.code = get_command_code("CMPGEQ");
            break;
        case '==':
            throw_error_unless_correct_datatypes(data_type_is_number(params.l_expression->return_type) &&
                                                 data_type_is_number(params.r_expression->return_type));
            expression->return_type = SBYTE;
            expression->command.code = get_command_code("CMPEQ");
            break;
        case '!=':
            throw_error_unless_correct_datatypes(data_type_is_number(params.l_expression->return_type) &&
                                                 data_type_is_number(params.r_expression->return_type));
            expression->return_type = SBYTE;
            expression->command.code = get_command_code("CMPNEQ");
            break;
        case '&':
            throw_error_unless_correct_datatypes(data_type_is_number(params.l_expression->return_type) &&
                                                 data_type_is_number(params.r_expression->return_type));
            expression->return_type = SINT;
            expression->command.code = get_command_code("BAND");
            break;
        case '|':
            throw_error_unless_correct_datatypes(data_type_is_number(params.l_expression->return_type) &&
                                                 data_type_is_number(params.r_expression->return_type));
            expression->return_type = SINT;
            expression->command.code = get_command_code("BOR");
            break;
        case '^':
            throw_error_unless_correct_datatypes(data_type_is_number(params.l_expression->return_type) &&
                                                 data_type_is_number(params.r_expression->return_type));
            expression->return_type = SINT;
            expression->command.code = get_command_code("BXOR");
            break;
        case '&&':
            throw_error_unless_correct_datatypes(data_type_is_number(params.l_expression->return_type) &&
                                                 data_type_is_number(params.r_expression->return_type));
            expression->return_type = SBYTE;
            expression->command.code = get_command_code("AND");
            break;
        case '||':
            throw_error_unless_correct_datatypes(data_type_is_number(params.l_expression->return_type) &&
                                                 data_type_is_number(params.r_expression->return_type));
            expression->return_type = SBYTE;
            expression->command.code = get_command_code("OR");
            break;
        case '=':
            handle_variable_assignment(expression);
            break;
    }
}

int operator_priority(char *op) {
    int op_number = string_to_int(op);

    switch (op_number) {
        case '*': case '/': case '%':
            return 3;
        case '+': case '-':
            return 4;
        case '<<': case '>>':
            return 5;
        case '>': case '<': case '>=': case '<=':
            return 6;
        case '==': case '!=':
            return 7;
        case '&':
            return 8;
        case '^':
            return 9;
        case '|':
            return 10;
        case '&&':
            return 11;
        case '||':
            return 12;
        case '=':
            return 14;
        default:
            return 0;
    }
}
