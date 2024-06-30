#include <stdio.h>
#include "include/lexer.h"
#include "include/parser.h"
#include "include/ast.h"
#include "include/defined_functions.h"

int main() {
    lexer_T *lexer = init_lexer("12 12 :+ 16 :+");
    parser_T *parser = init_parser(lexer);
    ast_T *ast = parser_parse(parser);
    //print_ast(ast);
    generate_defined_functions_hashmap();
    return 0;
}