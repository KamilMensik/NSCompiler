#include "include/lexer.h"
#include "include/predicates.h"
#include <stdlib.h>
#include <string.h>

lexer_T* init_lexer(char *contents) {
    lexer_T *lexer = calloc(1, sizeof(struct LEXER_STRUCT));
    lexer->contents = contents;
    lexer->i = 0;
    lexer->c = contents[0];
    lexer->contents_len = strlen(contents);

    return lexer;
}

void lexer_next_char(lexer_T *lexer) {
    if (lexer->c != '\0' && lexer->i < lexer->contents_len) {
        lexer->i += 1;
        lexer->c = lexer->contents[lexer->i];
    }
}

void lexer_skip_whitespace(lexer_T *lexer) {
    while (lexer->c == ' ') {
        lexer_next_char(lexer);
    }
}

token_T *lexer_get_next_token(lexer_T *lexer) {
    while (lexer->c != '\0' && lexer->i < lexer->contents_len) {
        if (lexer->c == ' ')
            lexer_skip_whitespace(lexer);

        if (is_number(lexer->c)) return lexer_collect_token(lexer, 0, 0, TOKEN_NUMBER, is_number);

        switch (lexer->c) {
            case ',': return lexer_collect_char_token(lexer, TOKEN_DIVIDER);
            case '\n': return lexer_collect_char_token(lexer, TOKEN_ENDLINE);
            case '.': return lexer_collect_token(lexer, 1, 0, TOKEN_REGION, is_symbol);
            case ':': return lexer_collect_token(lexer, 1, 0, TOKEN_FUNCTION, is_symbol);
            default: return lexer_collect_token(lexer, 0, 0, TOKEN_IDENTIFIER, is_symbol);
        }
    }

    return NULL;
}

token_T *lexer_collect_token(lexer_T *lexer, int skip_first_char, int skip_after_last, int token_type, int (*condition)(char)) {
    if (skip_first_char) lexer_next_char(lexer);

    int token_value_size = 8;
    char *token_value = malloc(sizeof(char) * token_value_size);

    for (int i = 0; ; i++) {
        if (i >= token_value_size) {
            token_value_size *= 2;
            realloc(token_value, token_value_size * sizeof(char));
        }

        if (condition(lexer->c)) {
            token_value[i] = lexer->c;
            lexer_next_char(lexer);
        } else {
            token_value[i] = '\0';
            break;
        }
    }

    if (skip_after_last) lexer_next_char(lexer);
    return init_token(token_type, token_value);
}

token_T *lexer_collect_char_token(lexer_T *lexer, int type) {
    char string[2] = { lexer->c, '\0'};
    lexer_next_char(lexer);

    return init_token(type, string);
}