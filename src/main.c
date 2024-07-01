#include <stdio.h>
#include "include/lexer.h"
#include "include/parser.h"
#include "include/ast.h"
#include "include/defined_functions.h"
#include "include/hashmap.h"

int main() {
    hashmap_T *defined_functions = generate_defined_functions_hashmap();

    lexer_T *lexer = init_lexer("12 12 :+ 16 :+");
    parser_T *parser = init_parser(lexer, defined_functions);
    ast_T *ast = parser_parse(parser);
    print_ast(ast);
    return 0;
}