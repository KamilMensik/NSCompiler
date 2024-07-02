#include "include/token.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

token_T *init_token(int type, char *value) {
    token_T *token = calloc(1, sizeof(struct TOKEN_STRUCT));
    token->type = type;

    char *kept_value;

    if (strcmp(value, "\n") == 0) {
        kept_value = malloc(sizeof(char) * 8);
        strcpy(kept_value, "NEWLINE");
    } else {
        kept_value = malloc(strlen(value) + 1);
        strcpy(kept_value, value);
    }

    token->value = kept_value;
    return token;
}

void print_token(token_T *token) {
    char *readable_type;
    switch (token->type) {
        case TOKEN_NUMBER:
            readable_type = "NUMBER";
            break;
        case TOKEN_IDENTIFIER:
            readable_type = "IDENTIFIER";
            break;
        case TOKEN_FUNCTION:
            readable_type = "FUNCTION";
            break;
        case TOKEN_DIVIDER:
            readable_type = "DIVIDER";
            break;
        case TOKEN_ENDLINE:
            readable_type = "ENDLINE";
            break;
        case TOKEN_REGION:
            readable_type = "REGION";
            break;
    }

    printf("(TOKEN %s %s)", readable_type, token->value);
}