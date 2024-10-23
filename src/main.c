#include <stdio.h>
#include "include/data_types.h"
#include "include/defined_functions.h"
#include "include/lexer.h"
#include "include/parser.h"
#include "include/ast.h"
#include "include/semantic_analyzer.h"
#include "include/nyassembly_builder.h"
#include "include/symbols.h"
#include <time.h>

int main(int argc, char **argv) {
    init_symbols();
    init_defined_functions();
    clock_t start, end;
    double total_time_used;
    start = clock();

    if (argc < 2) {
        printf("You must pass a file, as a argument!\n");
        exit(1);
    }

    FILE *input = fopen(argv[1], "r");

    if (input == NULL) {
        printf("Could not find file %s!\n", argv[1]);
        exit(1);
    }
    
    lexer_T *lexer = init_lexer(input, argv[1]);

    parser_T *parser = init_parser(lexer);
    ast_T *ast = parser_parse(parser);

    /*
    No need to print the AST Tree now, just wasting speed.
    FILE *ast_print = fopen("test.txt", "w");

    print_ast(ast, ast_print, NULL);

    fclose(ast_print);
    */

    semantic_analyzer_T *semantic_analyzer = init_semantic_analyzer();
    semantic_analyze(semantic_analyzer, ast);

    FILE *output = fopen("output.bin", "wb");
    FILE *nyassembly_output = fopen("output.nyassembly", "w");
    build_nyassembly(ast, output, nyassembly_output);

    fclose(input);
    fclose(output);
    fclose(nyassembly_output);

    end = clock();
    total_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

    printf("File compiled successfully in %.5fs\n", total_time_used);

    // FREE EVERYTHING AFTER RUN
    free_ast(ast, 1);
    free_symbols();
    free_semantic_analyzer(semantic_analyzer);
    free_parser(parser, 1);
    free_keywords();
    free_data_type_conversion_table();
    free_command_codes();

    return 0;
}
