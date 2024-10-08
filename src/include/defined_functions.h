#ifndef DEFINED_FUNCTIONS_H
#define DEFINED_FUNCTIONS_H

#include "hashmap.h"

typedef enum {
    DATA_INT
} DATA_TYPE;

typedef struct DEFINED_FUNCTION_VARIATION_STRUCT {
    int *parameters;
    int *output;
    unsigned int code;
} defined_function_variation_T;

typedef struct DEFINED_FUNCTION_STRUCT {
    unsigned int parameter_count;
    unsigned int output_count;
    unsigned int variation_count;
    defined_function_variation_T **variations;
} defined_function_T;

hashmap_T *generate_data_type_conversion_table();

DATA_TYPE *get_data_type(hashmap_T *data_type_conversion_table, char *string);

void generate_defined_functions_hashmap();

void defined_functions_hashmap_set(char *key, defined_function_T *defined_function);

defined_function_T *defined_functions_hashmap_get(char *key);

#endif
