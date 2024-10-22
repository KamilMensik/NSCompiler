#ifndef PREDICATES_H
#define PREDICATES_H

int is_symbol(char c, int can_be_underscore);

int is_number(char c, int unused);

int is_not_quote(char c, int unused);

#endif
