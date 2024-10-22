#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H

#include "ast.h"
#include "hashmap.h"

typedef struct SEMANTIC_ANALYZER_STRUCT {
    char *context;
    unsigned int defined_function_sizes;
} semantic_analyzer_T;

semantic_analyzer_T *init_semantic_analyzer();

void free_semantic_analyzer(semantic_analyzer_T *analyzer);

ast_T *semantic_analyze(semantic_analyzer_T *analyzer, ast_T *programme);

#endif
