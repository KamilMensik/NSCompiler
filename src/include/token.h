#ifndef TOKEN_H
#define TOKEN_H

typedef struct TOKEN_STRUCT {
    enum {
        TOKEN_IDENTIFIER,
        TOKEN_NUMBER,
        TOKEN_REGION,
        TOKEN_FUNCTION,
        TOKEN_DIVIDER,
        TOKEN_ENDLINE
    } type;
    char *value;
} token_T;

token_T *init_token(int type, char *value);

void print_token(token_T *token);

#endif
