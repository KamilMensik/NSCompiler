#include "include/semantic_analyzer.h"
#include "include/defined_functions.h"
#include <stdio.h>

semantic_analyzer_T *init_semantic_analyzer(ast_T *ast) {
    semantic_analyzer_T *analyzer = malloc(sizeof(struct SEMANTIC_ANALYZER_STRUCT));
    analyzer->ast = ast;

    return analyzer;
}

/*

void analyze_ast_atom(ast_T *ast) {
    switch (ast->token->type) {
        case TOKEN_NUMBER:
            ast->additional_data = atoi(ast->token->value);
            if (ast->additional_data < 64) {
                ast->additional_data_position = ADDITIONAL_DATA_INSIDE;
                ast->size_bytes = 2;
            } else {
                ast->additional_data_position = ADDITIONAL_DATA_BEHIND;
                ast->size_bytes = 4;
            }
            ast->assigned_defined_function = defined_functions_hashmap_get("lnum");
            ast->selected_defined_function_variation = 0;
            break;
    }
}

void analyze_ast_compound(ast_T *ast) {
    unsigned int size = 0;

    for (int i = 0; i < ast->children_count; i++) {
        analyze_ast(ast->children[i]);
        size += ast->children[i]->size_bytes;
    }

    switch (ast->token->type) {
        case TOKEN_FUNCTION:
            ast->assigned_defined_function = defined_functions_hashmap_get(ast->token->value);
            int variation_index = static_type_check(ast);
            if (variation_index == -1) {
                printf("Could not find variation of function %s with selected parameters.", ast->token->value);
                exit(1);
            } else {
                ast->selected_defined_function_variation = variation_index;
            }
            size += 2;
            ast->size_bytes = size;
            break;
        case TOKEN_CONDITIONAL:
            if (ast->token->value[0] == 'e') {
                defined_function_T *function = malloc(sizeof(struct DEFINED_FUNCTION_STRUCT));
                function->variation_count = 1;
                function->variations = malloc(sizeof(struct DEFINED_FUNCTION_VARIATION_STRUCT *));
                defined_function_variation_T *variation = malloc(sizeof(struct DEFINED_FUNCTION_VARIATION_STRUCT));
                variation->code = 5;
                variation->output = malloc(sizeof(int) * ast->output_count);
                int variation_output_index = 0;

                for (int i = 0; i < ast->children_count; i++) {
                    ast_T *child = ast->children[i];
                    defined_function_variation_T *child_variation = child->assigned_defined_function->variations[child->selected_defined_function_variation];

                    for (int j = 0; j < child->output_count; j++)
                        variation->output[variation_output_index++] = child_variation->output[j];
                }

                ast->selected_defined_function_variation = 0;
                function->variations[0] = variation;
                ast->assigned_defined_function = function;
                size += 2;
            } else {
                int variation_output_index = 0;

                for (int i = 0; i < ast->children_count; i++) {
                    ast_T *child = ast->children[i];
                    defined_function_variation_T *child_variation = child->assigned_defined_function->variations[child->selected_defined_function_variation];

                    for (int j = 0; j < child->output_count; j++) {
                        if (child_variation->output[j] != ast->assigned_defined_function->variations[0]->output[variation_output_index++]) {
                            printf("Inconsistent condition output!\n");
                            exit(1);
                        }
                    }
                }

                ast->assigned_defined_function = NULL;
            }
            ast->size_bytes = size;
    }
}

void analyze_ast_conditional(ast_T *ast) {
    unsigned int size = 0;
    ast->assigned_defined_function = malloc(sizeof(struct DEFINED_FUNCTION_STRUCT));
    ast->selected_defined_function_variation = 0;
    ast->assigned_defined_function->variations = malloc(sizeof(struct DEFINED_FUNCTION_VARIATION_STRUCT *));
    ast->assigned_defined_function->variations[0] = malloc(sizeof(struct DEFINED_FUNCTION_VARIATION_STRUCT));
    ast->assigned_defined_function->variations[0]->code = 4;

    analyze_ast(ast->children[0]);
    size += ast->children[0]->size_bytes;
    analyze_ast_compound(ast->children[1]);
    ast->children[2]->assigned_defined_function = ast->children[1]->assigned_defined_function;
    analyze_ast_compound(ast->children[2]);
    ast->assigned_defined_function->variations[0]->output = ast->children[1]->assigned_defined_function->variations[0]->output;

    size += ast->children[1]->size_bytes + ast->children[2]->size_bytes;
    ast->additional_data = ast->children[1]->size_bytes;
    ast->children[1]->additional_data = ast->children[2]->size_bytes;

    size += 2;
    ast->size_bytes = size;
}

int static_type_check(ast_T *ast) {
    defined_function_T *function = ast->assigned_defined_function;

    int *parameters = malloc(sizeof(int) * function->parameter_count);
    int current_parameter_index = 0;
    defined_function_variation_T *variation;

    for (int i = 0; i < ast->children_count; i++) {
        ast_T *child = ast->children[i];
        switch (child->type) {
            case AST_ATOM: case AST_COMPOUND:
                variation = child->assigned_defined_function->variations[child->selected_defined_function_variation];
                for (int j = 0; j < child->assigned_defined_function->output_count; j++) {
                    parameters[current_parameter_index++] = variation->output[j];
                }
                break;
        }
    }

    int found_variation_index = -1;

    for (int i = 0; i < function->variation_count; i++) {
        if (found_variation_index != -1)
            break;

        defined_function_variation_T *variation = function->variations[i];
        for (int j = 0; j <= current_parameter_index; j++) {
            if (j == current_parameter_index)
                found_variation_index = i;
            else if (variation->parameters[j] != parameters[j])
                break;
        }
    }

    return found_variation_index;
}

void analyze_ast(ast_T *ast) {
    switch (ast->type) {
        case AST_ATOM:
            analyze_ast_atom(ast);
            break;
        case AST_COMPOUND:
            analyze_ast_compound(ast);
            break;
        case AST_CONDITIONAL:
            analyze_ast_conditional(ast);
            break;
    }
}

ast_T *semantic_analyze(semantic_analyzer_T *analyzer) {
    analyze_ast(analyzer->ast);

    return analyzer->ast;
}

*/