#include "include/lexer.h"
#include "include/predicates.h"
#include "include/token.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

lexer_T* init_lexer(FILE *file) {
    lexer_T *lexer = calloc(1, sizeof(struct LEXER_STRUCT));
    lexer->file = file;
    lexer_next_char(lexer);
    lexer->char_index = 0;
    lexer->line = 1;
    lexer->should_get_next_character = 0;
    lexer->peeked_token = NULL;

    return lexer;
}

void lexer_next_char(lexer_T *lexer) {
    if (!feof(lexer->file)) {
        lexer->char_index += 1;
        lexer->c = toupper(fgetc(lexer->file));
    }
}

void lexer_skip_whitespace(lexer_T *lexer) {
    while (lexer->c == ' ') {
        lexer_next_char(lexer);
    }
}

token_T *lexer_peek(lexer_T *lexer) {
    if (lexer->peeked_token == NULL)
        lexer->peeked_token = lexer_get_next_token(lexer);

    return lexer->peeked_token;
}

token_T *lexer_get_next_token(lexer_T *lexer) {
    if (lexer->peeked_token != NULL) {
        token_T *token = lexer->peeked_token;
        lexer->peeked_token = NULL;
        return token;
    }


    while (1) {
        if (lexer->should_get_next_character == 0) {
            lexer->should_get_next_character = 1;
        } else {
            lexer_next_char(lexer);
        }

        if (feof(lexer->file)) break;

        if (lexer->c == ' ')
            lexer_skip_whitespace(lexer);

        if (lexer->c == '"') return lexer_collect_token(lexer, 1, 1, TOKEN_STRING, is_not_quote);
        if (is_number(lexer->c, 0)) return lexer_collect_token(lexer, 0, 0, TOKEN_NUMBER, is_number);
        if (is_symbol(lexer->c, 0)) return lexer_collect_token(lexer, 0, 0, TOKEN_IDENTIFIER, is_symbol);
        if (lexer->c == '\n') {
                lexer->char_index = 0;
                lexer->line+=1;
                return lexer_get_next_token(lexer);
        }

        return lexer_collect_operator(lexer);
    }

    return init_token(TOKEN_EOF, "EOF", lexer->line, lexer->char_index);
}

token_T *lexer_collect_token(lexer_T *lexer, int skip_first_char, int skip_after_last, int token_type, int (*condition)(char, int)) {
    int cached_char_index = lexer->char_index;
    if (skip_first_char) lexer_next_char(lexer);

    int token_value_size = 8;
    char *token_value = malloc(sizeof(char) * token_value_size);

    for (int i = 0; ; i++) {
        if (i >= token_value_size) {
            token_value_size *= 2;
            token_value = realloc(token_value, token_value_size * sizeof(char));
        }

        if (condition(lexer->c, 1)) {
            token_value[i] = lexer->c;
            lexer_next_char(lexer);
        } else {
            token_value[i] = '\0';
            break;
        }
    }

    printf("%s\n", token_value);

    if (!skip_after_last) lexer->should_get_next_character = 0;

    return init_token(token_type, token_value, lexer->line, cached_char_index);
}

token_T *lexer_collect_operator(lexer_T *lexer) {
    char *token_value;
    int token_type;

    switch (lexer->c) {
        case '+':
            lexer_next_char(lexer);
            if (lexer->c == '+')
                return init_token(TOKEN_UNARY_OPERATOR, "++", lexer->line, lexer->char_index);
            
            lexer->should_get_next_character = 0;
            return init_token(TOKEN_BINARY_OPERATOR, "+", lexer->line, lexer->char_index);
        case '-':
            lexer_next_char(lexer);
            if (lexer->c == '-')
                return init_token(TOKEN_UNARY_OPERATOR, "--", lexer->line, lexer->char_index);
            else if (is_number(lexer->c, 0)) {
                lexer->should_get_next_character = 0;
                token_T *token = lexer_get_next_token(lexer);
                char *str = malloc(sizeof(char) * (strlen(token->value) + 2));
                str[0] = '-';
                str[1] = '\0';
                strcat(str, token->value);
                free(token->value);
                token->value = str;
                return token;
            }
            
            lexer->should_get_next_character = 0;
            return init_token(TOKEN_BINARY_OPERATOR, "-", lexer->line, lexer->char_index);
        case '*':
            return init_token(TOKEN_BINARY_OPERATOR, "*", lexer->line, lexer->char_index);
        case '/':
            return init_token(TOKEN_BINARY_OPERATOR, "/", lexer->line, lexer->char_index);
        case '%':
            return init_token(TOKEN_BINARY_OPERATOR, "%", lexer->line, lexer->char_index);
        case '=':
            lexer_next_char(lexer);
            if (lexer->c == '=')
                return init_token(TOKEN_BINARY_OPERATOR, "==", lexer->line, lexer->char_index);
            
            lexer->should_get_next_character = 0;
            return init_token(TOKEN_BINARY_OPERATOR, "=", lexer->line, lexer->char_index);
        case '>':
            lexer_next_char(lexer);
            if (lexer->c == '=')
                return init_token(TOKEN_BINARY_OPERATOR, ">=", lexer->line, lexer->char_index);
            if (lexer->c == '<')
                return init_token(TOKEN_BINARY_OPERATOR, ">>", lexer->line, lexer->char_index);
            
            lexer->should_get_next_character = 0;
            return init_token(TOKEN_BINARY_OPERATOR, ">", lexer->line, lexer->char_index);

        case '<':
            lexer_next_char(lexer);
            if (lexer->c == '=')
                return init_token(TOKEN_BINARY_OPERATOR, "<=", lexer->line, lexer->char_index);
            if (lexer->c == '<')
                return init_token(TOKEN_BINARY_OPERATOR, "<<", lexer->line, lexer->char_index);
            
            lexer->should_get_next_character = 0;
            return init_token(TOKEN_BINARY_OPERATOR, "<", lexer->line, lexer->char_index);

        case '!':
            lexer_next_char(lexer);
            if (lexer->c == '=')
                return init_token(TOKEN_BINARY_OPERATOR, "!=", lexer->line, lexer->char_index);
            
            lexer->should_get_next_character = 0;
            return init_token(TOKEN_UNARY_OPERATOR, "!", lexer->line, lexer->char_index);
        case '&':
            lexer_next_char(lexer);
            if (lexer->c == '&')
                return init_token(TOKEN_BINARY_OPERATOR, "&&", lexer->line, lexer->char_index);
            
            lexer->should_get_next_character = 0;
            return init_token(TOKEN_BINARY_OPERATOR, "&", lexer->line, lexer->char_index);
        case '|':
            lexer_next_char(lexer);
            if (lexer->c == '|')
                return init_token(TOKEN_BINARY_OPERATOR, "||", lexer->line, lexer->char_index);
            
            lexer->should_get_next_character = 0;
            return init_token(TOKEN_BINARY_OPERATOR, "|", lexer->line, lexer->char_index);
        case '^': case '~':
            return lexer_collect_char_token(lexer, TOKEN_BINARY_OPERATOR);
        case '(': case ')':
            return lexer_collect_char_token(lexer, TOKEN_PARENTHESIS);
        case '[': case ']':
            return lexer_collect_char_token(lexer, TOKEN_SQUARE_BRACKET);
        case '{': case '}':
            return lexer_collect_char_token(lexer, TOKEN_CURLY_BRACE);
        case ';':
            return lexer_collect_char_token(lexer, TOKEN_SEMICOLON);
        case ',':
            return lexer_collect_char_token(lexer, TOKEN_DIVIDER);
    }

    printf("UNKNOWN OPERATOR! %c ABCD", lexer->c);
    exit(1);
}

token_T *lexer_collect_char_token(lexer_T *lexer, int type) {
    char *string = malloc(sizeof(char) * 2);
    string[0] = lexer->c;
    string[1] = '\0';

    return init_token(type, string, lexer->line, lexer->char_index);
}
