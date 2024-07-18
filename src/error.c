#include "include/error.h"
#include <stdio.h>
#include <stdlib.h>

void throw_token_error(token_T *token, char *error_message) {
    printf("Error detected on line %d:%d. %s Got: ", token->line, token->char_index, error_message);
    print_token(token);
    exit(1);
}

void throw_error(char *message) {
    printf("%s", message);
    exit(1);
}