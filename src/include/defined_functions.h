#ifndef DEFINED_FUNCTIONS_H
#define DEFINED_FUNCTIONS_H

#include "hashmap.h"

typedef enum {
    DATA_INT
} DATA_TYPE;

typedef struct DEFINED_FUNCTION_VARIATION_STRUCT {
    DATA_TYPE *parameters;
    DATA_TYPE *output;
    unsigned int code;
} defined_function_variation_T;

typedef struct DEFINED_FUNCTION_STRUCT {
    unsigned int parameter_count;
    unsigned int output_count;
    unsigned int variation_count;
    defined_function_variation_T **variations;
} defined_function_T;

defined_function_T *generate_function(int parameter_count, int output_count, int variations_count, ...);

hashmap_T *generate_data_type_conversion_table();

DATA_TYPE *get_data_type(hashmap_T *data_type_conversion_table, char *string);

hashmap_T *generate_defined_functions_hashmap();

#endif