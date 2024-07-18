#ifndef ERROR_H
#define ERROR_H

#include "token.h"

void throw_token_error(token_T *token, char *message);

void throw_error(char *message);

#endif