#include "include/nyassembly_builder.h"
#include "include/ast.h"

short swap_endian(short n) {
    int c = n >> 8;
    return ((n & 255) << 8) + c;
}

void build_nyassembly(ast_T *ast, FILE *output) {
    short additional_data;
    switch (ast->type) {
        case AST_COMPOUND:
            for (int i = 0; i < ast->children_count; i++) {
                build_nyassembly(ast->children[i], output);
            }
            switch (ast->additional_data_position) {
                case ADDITIONAL_DATA_INSIDE:
                    fwrite(&ast->assigned_defined_function->variations[ast->selected_defined_function_variation]->code, sizeof(char), 1, output);
                    fwrite(&ast->additional_data, sizeof(char), 1, output);
                    break;
                case ADDITIONAL_DATA_BEHIND:
                    fwrite(&ast->additional_data, sizeof(short), 1, output);
                    break;
            }
            break;
        case AST_ATOM:
            fwrite(&ast->assigned_defined_function->variations[ast->selected_defined_function_variation]->code, sizeof(short), 1, output);
            switch (ast->additional_data_position) {
                case ADDITIONAL_DATA_BEHIND:
                    additional_data = swap_endian(ast->additional_data);
                    fwrite(&additional_data, sizeof(short), 1, output);
                    break;
            }
            break;
    }
}