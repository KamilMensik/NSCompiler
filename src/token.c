#include "include/token.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

hashmap_T *keywords = NULL;

token_T *init_token(int type, char *value, int line, int char_index) {
    token_T *token = calloc(1, sizeof(struct TOKEN_STRUCT));
    token->type = type;

    char *kept_value;

    if (type == TOKEN_ENDLINE) {
        kept_value = malloc(sizeof(char) * 8);
        strcpy(kept_value, "NEWLINE");
    } else {
        kept_value = malloc(strlen(value) + 1);
        strcpy(kept_value, value);
    }

    if (type == TOKEN_IDENTIFIER) {
        if (keywords == NULL)
            keywords = init_keywords();

        char *res = (char *)hashmap_get(keywords, value);
        if (res != NULL) {
            token->type = TOKEN_KEYWORD;
            token->keyword_id = (int)*res;
        }
    }

    token->value = kept_value;
    token->line = line;
    token->char_index = char_index;
    return token;
}

hashmap_T *init_keywords() {
    hashmap_T *hashmap = new_hashmap(50);
    
    FILE *file = fopen("src/config/keywords.cfg", "r");
    char buff[32];
    int id = 0;
    for (int i = 0; ; i++) {
        char c = fgetc(file);

        if (c == ' ' || c == '\n' || feof(file)) {
            buff[i] = '\0';
            int *e = malloc(sizeof(int));
            *e = id++;
            i = -1;
            
            hashmap_set(hashmap, buff, e);
            if (feof(file))
                break;
        } else {
            buff[i] = c;
        }
    }

    return hashmap;
}

token_T *token_pop(stack_T *stack) {
    return (struct TOKEN_STRUCT *)(stack_pop(stack));
}

token_T *token_peek(stack_T *stack) {
    return (struct TOKEN_STRUCT *)(stack_peek(stack));
}

void token_push(stack_T *stack, token_T *token) {
    stack_push(stack, token);
}

token_T **token_stack_to_array(stack_T *stack, int keep_stack) {
    token_T **tokens = malloc(sizeof(struct TOKEN_STRUCT *) * stack->size);

    for (int i = stack->size - 1; i >= 0; i--) {
        tokens[i] = token_pop(stack);
    }

    if (!keep_stack) free(stack);

    return tokens;
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
        case TOKEN_DIVIDER:
            readable_type = "DIVIDER";
            break;
        case TOKEN_ENDLINE:
            readable_type = "ENDLINE";
            break;
            break;
        case TOKEN_UNARY_OPERATOR:
            readable_type = "UNARY OPERATOR";
            break;
        case TOKEN_BINARY_OPERATOR:
            readable_type = "BINARY OPERATOR";
            break;
        case TOKEN_ASSIGNMENT:
            readable_type = "ASSIGNMENT";
            break;
        case TOKEN_PARENTHESIS:
            readable_type = "PARENTHESIS";
            break;
        case TOKEN_SQUARE_BRACKET:
            readable_type = "SQUARE BRACKET";
            break;
        case TOKEN_CURLY_BRACE:
            readable_type = "CURLY BRACE";
            break;
        case TOKEN_SEMICOLON:
            readable_type = "SEMICOLON";
            break;
        case TOKEN_KEYWORD:
            readable_type = "KEYWORD";
            break;
        case TOKEN_EOF:
            readable_type = "EOF";
            break;
    }

    if (token->type == TOKEN_KEYWORD)
        printf("(TOKEN %s %s KEYWORD_ID: %d)", readable_type, token->value, token->keyword_id);
    else
        printf("(TOKEN %s %s)", readable_type, token->value);
}

int token_data_type(token_T *token) {
    return (token->type == TOKEN_KEYWORD) || token->keyword_id >= 7;
}