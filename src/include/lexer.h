#ifndef LEXER_H
#define LEXER_H
#include "token.h"
#include <stdio.h>

typedef struct LEXER_STRUCT {
    char c;
    unsigned int char_index;
    unsigned int line;
    FILE *file;
    char *filename;
    unsigned char should_get_next_character;
    token_T *peeked_token;
    unsigned char collecting_string;
    token_T *last_token;
} lexer_T;

lexer_T* init_lexer(FILE *file, char *filename);

void free_lexer(lexer_T *lexer);

void lexer_next_char(lexer_T *lexer);

void lexer_skip_whitespace(lexer_T *lexer);

token_T *lexer_peek(lexer_T *lexer);

token_T *lexer_get_next_token(lexer_T *lexer);

token_T *lexer_collect_operator(lexer_T *lexer);

token_T *lexer_collect_token(lexer_T *lexer, int skip_first_char, int skip_after_last, int token_type, int (*condition)(char, int));

token_T *lexer_collect_char_token(lexer_T *lexer, int type);
#endif
