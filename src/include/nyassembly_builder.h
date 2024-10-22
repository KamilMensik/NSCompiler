#ifndef NYASSEMBLY_BUILDER_H
#define NYASSEMBLY_BUILDER_H

#include "ast.h"
#include <stdio.h>

short swap_endian(short n);

void build_nyassembly(ast_T *ast, FILE *output, FILE *nyassembly_output);

#endif
