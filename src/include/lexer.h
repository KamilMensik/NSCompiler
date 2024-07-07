#ifndef LEXER_H
#define LEXER_H
#include "token.h"
#include <stdio.h>

typedef struct LEXER_STRUCT {
    char c;
    unsigned int i;
    FILE *file;
    unsigned int contents_len;
} lexer_T;

lexer_T* init_lexer(FILE *file);

void lexer_next_char(lexer_T *lexer);

void lexer_skip_whitespace(lexer_T *lexer);

token_T *lexer_get_next_token(lexer_T *lexer);

token_T *lexer_collect_token(lexer_T *lexer, int skip_first_char, int skip_after_last, int token_type, int (*condition)(char));

token_T *lexer_collect_char_token(lexer_T *lexer, int type);
#endif