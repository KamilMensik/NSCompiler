#include "include/predicates.h"

int is_symbol(char c) {
    return (c > 41 && c != 44 && c < 140);
}

int is_number(char c) {
    return (c >= 48 && c <= 57);
}