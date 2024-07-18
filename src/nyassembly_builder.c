#include "include/nyassembly_builder.h"
#include "include/ast.h"

/*
short swap_endian(short n) {
    int c = n >> 8;
    return ((n & 255) << 8) + c;
}

void build_nyassembly(ast_T *ast, FILE *output) {
    short additional_data;
    char function_code = ast->assigned_defined_function->variations[ast->selected_defined_function_variation]->code + 128 * ast->additional_data_position;
    switch (ast->type) {
        case AST_COMPOUND:
            for (int i = 0; i < ast->children_count; i++) {
                build_nyassembly(ast->children[i], output);
            }
            switch (ast->additional_data_position) {
                case ADDITIONAL_DATA_INSIDE:
                    fwrite(&function_code, sizeof(char), 1, output);
                    fwrite(&ast->additional_data, sizeof(char), 1, output);
                    break;
                case ADDITIONAL_DATA_BEHIND:
                    printf("Not implemented warning!\n");
                    break;
            }
            break;
        case AST_ATOM:
            switch (ast->additional_data_position) {
                case ADDITIONAL_DATA_INSIDE:
                    fwrite(&function_code, sizeof(char), 1, output);
                    fwrite(&ast->additional_data, sizeof(char), 1, output);
                    break;
                case ADDITIONAL_DATA_BEHIND:
                    fwrite(&function_code, sizeof(short), 1, output);
                    additional_data = swap_endian(ast->additional_data);
                    fwrite(&additional_data, sizeof(short), 1, output);
                    break;
            }
            break;
        case AST_CONDITIONAL:
            build_nyassembly(ast->children[0], output);
            switch (ast->additional_data_position) {
                case ADDITIONAL_DATA_INSIDE:
                    fwrite(&function_code, sizeof(char), 1, output);
                    fwrite(&ast->additional_data, sizeof(char), 1, output);
                    break;
                case ADDITIONAL_DATA_BEHIND:
                    printf("Not implemented warning!\n");
                    break;
            }
            build_nyassembly(ast->children[1], output);
            for(int i = 0; i < ast->children[2]->children_count; i++)
                build_nyassembly(ast->children[2]->children[i], output);

            break;
    }
}
*/