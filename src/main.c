#include <stdio.h>
#include "include/lexer.h"
#include "include/parser.h"
#include "include/ast.h"
#include "include/defined_functions.h"
#include "include/hashmap.h"
#include "include/semantic_analyzer.h"

int main() {
    generate_defined_functions_hashmap();

    lexer_T *lexer = init_lexer("12 12 :+ 16 :+");
    parser_T *parser = init_parser(lexer);
    ast_T *ast = parser_parse(parser);

    semantic_analyzer_T *semantic_analyzer = init_semantic_analyzer(ast);
    semantic_analyze(semantic_analyzer);
    printf("%d", ast->size_bytes);
    return 0;
}