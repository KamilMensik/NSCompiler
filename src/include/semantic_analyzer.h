#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H

#include "ast.h"
#include "hashmap.h"

typedef struct SEMANTIC_ANALYZER_STRUCT {
    ast_T *ast;
} semantic_analyzer_T;

semantic_analyzer_T *init_semantic_analyzer(ast_T *ast);

void analyze_ast_atom(ast_T *ast);

void analyze_ast_compound(ast_T *ast);

void analyze_ast_conditional(ast_T *ast);

int static_type_check(ast_T *ast);

void analyze_ast(ast_T *ast);

ast_T *semantic_analyze(semantic_analyzer_T *analyzer);

#endif