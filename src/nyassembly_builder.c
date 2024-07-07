#include "include/nyassembly_builder.h"
#include "include/ast.h"

void build_nyassembly(ast_T *ast, FILE *output) {
    switch (ast->type) {
        case AST_COMPOUND:
            for (int i = 0; i < ast->children_count; i++) {
                build_nyassembly(ast->children[i], output);
            }
            fwrite(&ast->assigned_defined_function->variations[ast->selected_defined_function_variation]->code, sizeof(short), 1, output);
            switch (ast->additional_data_position) {
                case ADDITIONAL_DATA_BEHIND:
                    fwrite(&ast->additional_data, sizeof(short), 1, output);
            }
            break;
        case AST_ATOM:
            fwrite(&ast->assigned_defined_function->variations[ast->selected_defined_function_variation]->code, sizeof(short), 1, output);
            switch (ast->additional_data_position) {
                case ADDITIONAL_DATA_BEHIND:
                    fwrite(&ast->additional_data, sizeof(short), 1, output);
            }
            break;
    }
}