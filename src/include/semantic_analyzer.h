#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H

#include "ast.h"

typedef struct SEMANTIC_ANALYZER_STRUCT {
    ast_T *ast;
} semantic_analyzer_T;

ast_T *semantic_analyze(semantic_analyzer_T *analyzer);

#endif