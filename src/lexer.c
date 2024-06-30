#include "include/lexer.h"
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

void lexer_advance(lexer_T *lexer) {
    if (lexer->c != '\0' && lexer->i < lexer->contents_len) {
        lexer->i += 1;
        lexer->c = lexer->contents[lexer->i];
    }
}

void lexer_skip_whitespace(lexer_T *lexer) {
    while (lexer->c == ' ') {
        lexer_advance(lexer);
    }
}

token_T *lexer_get_next_token(lexer_T *lexer) {
    while (lexer->c != '\0' && lexer->i < lexer->contents_len) {
        if (lexer->c == ' ')
            lexer_skip_whitespace(lexer);

        if (lexer->c >= 48 && lexer->c <= 57) return lexer_collect_number(lexer);

        switch (lexer->c) {
            case ',': return lexer_advance_with_token(lexer, init_token(TOKEN_DIVIDER, lexer_get_current_char_as_string(lexer)));
            case 10: return lexer_advance_with_token(lexer, init_token(TOKEN_ENDLINE, "NEWLINE"));
            case '.': return lexer_collect_region(lexer);
            case ':': return lexer_collect_function(lexer);
            default: return lexer_collect_identifier(lexer);
        }
    }

    return NULL;
}

token_T *lexer_collect_region(lexer_T *lexer) {
    lexer_advance(lexer);

    char *value = calloc(1, sizeof(char));
    value[0] = '\0';

    for (int i = 2; ; i++) {
        if (lexer->c == 10) break;
        char *s = lexer_get_current_char_as_string(lexer);
        value = realloc(value, i * sizeof(char));
        strcat(value, s);
        lexer_advance(lexer);
    }

    return init_token(TOKEN_REGION, value);
}

token_T *lexer_collect_identifier(lexer_T *lexer) {
    char *value = calloc(1, sizeof(char));
    value[0] = '\0';

    for (int i = 2; ; i++) {
        if (!(lexer->c > 41 && lexer-> c < 140)) break;
        char *s = lexer_get_current_char_as_string(lexer);
        value = realloc(value, i * sizeof(char));
        strcat(value, s);
        lexer_advance(lexer);
    }

    return init_token(TOKEN_IDENTIFIER, value);
}

token_T *lexer_collect_number(lexer_T *lexer) {
    char *value = calloc(1, sizeof(char));
    value[0] = '\0';

    for (int i = 2; ; i++) {
        if (!(lexer->c >= 48 && lexer-> c <= 57)) break;
        char *s = lexer_get_current_char_as_string(lexer);
        value = realloc(value, i * sizeof(char));
        strcat(value, s);
        lexer_advance(lexer);
    }

    return init_token(TOKEN_NUMBER, value);
}

token_T *lexer_collect_function(lexer_T *lexer) {
    char *value = calloc(1, sizeof(char));
    value[0] = '\0';

    for (int i = 2; ; i++) {
        if (!(lexer->c > 41 && lexer-> c < 140)) break;
        char *s = lexer_get_current_char_as_string(lexer);
        value = realloc(value, i * sizeof(char));
        strcat(value, s);
        lexer_advance(lexer);
    }

    return init_token(TOKEN_FUNCTION, value);
}

token_T *lexer_advance_with_token(lexer_T *lexer, token_T *token) {
    lexer_advance(lexer);

    return token;
}

char *lexer_get_current_char_as_string(lexer_T *lexer) {
    char *s = calloc(2, sizeof(char));
    s[0] = lexer->c;
    s[1] = '\0';
    return s;
}