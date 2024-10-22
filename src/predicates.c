#include "include/predicates.h"

int is_symbol(char c, int can_be_underscore) {
    return (is_number(c, 0) || (c >= 65 && c <= 90) || (can_be_underscore && c == 95));
}

int is_number(char c, int unused) {
    return (c >= 48 && c <= 57);
}

int is_not_quote(char c, int unused) {
    return (c != '"');
}
