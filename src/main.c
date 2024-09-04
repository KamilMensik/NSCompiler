#include <stdio.h>
#include "include/lexer.h"
#include "include/parser.h"
#include "include/ast.h"
#include "include/defined_functions.h"
#include "include/hashmap.h"
#include "include/semantic_analyzer.h"
#include "include/nyassembly_builder.h"
#include <time.h>

int main(int argc, char **argv) {
    clock_t start, end;
    double total_time_used;
    start = clock();
    generate_defined_functions_hashmap();

    if (argc < 2) {
        printf("You must pass a file, as a argument!\n");
        exit(1);
    }

    FILE *input = fopen(argv[1], "r");

    if (input == NULL) {
        printf("Could not find file %s!\n", argv[1]);
        exit(1);
    }
    
    lexer_T *lexer = init_lexer(input);

    parser_T *parser = init_parser(lexer);
    ast_T *ast = parser_parse(parser);

    FILE *ast_print = fopen("test.txt", "w");

    print_ast(ast, ast_print, NULL);

    fclose(ast_print);

    /*
    semantic_analyzer_T *semantic_analyzer = init_semantic_analyzer(ast);
    semantic_analyze(semantic_analyzer);

    FILE *output = fopen("output.bin", "wb");
    build_nyassembly(ast, output);
    */

    fclose(input);
    //fclose(output);

    end = clock();
    total_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

    printf("File compiled successfully in %.3fs\n", total_time_used);

    return 0;
}