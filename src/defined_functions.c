#include "include/defined_functions.h"
#include <stdarg.h>
#include <stdio.h>

/*
defined_function_T *generate_function(int parameter_count, int output_count, int variations_count, ...) {
    va_list list;
    int size = (parameter_count + output_count + 1) * variations_count;
    defined_function_variation_T **variations_array = malloc(sizeof(struct DEFINED_FUNCTION_VARIATION_STRUCT *) * variations_count);

    va_start(list, size);

    for (int i = 0; i < variations_count; i++) {
        defined_function_variation_T *new_variation = malloc(sizeof(struct DEFINED_FUNCTION_VARIATION_STRUCT));
        new_variation->parameters = malloc(sizeof(int) * parameter_count);
        new_variation->code = va_arg(list, int);
        for (int j = 0; j < parameter_count; j ++) {
            new_variation->parameters[j] = va_arg(list, int);;
        }

        new_variation->output = malloc(sizeof(int) * output_count);
        for (int j = 0; j < output_count; j ++) {
            new_variation->output[j] = va_arg(list, int);
        }
    }

    defined_function_T *function = malloc(sizeof(struct DEFINED_FUNCTION_STRUCT));
    function->parameter_count = parameter_count;
    function->output_count = output_count;
    function->variation_count = variations_count;
    function->variations = variations_array;

    va_end(list);

    return function;
}
*/

hashmap_T *generate_data_type_conversion_table() {
    hashmap_T *hashmap = new_hashmap(20);
    static int data_types[1] = { DATA_INT };
    hashmap_set(hashmap, "INT", data_types);

    return hashmap;
}

DATA_TYPE *get_data_type(hashmap_T *data_type_conversion_table, char *string) {
    return (DATA_TYPE *)hashmap_get(data_type_conversion_table, string);
}

void skip_until_character(FILE *file, char c) {
    if (fgetc(file) == c) return;

    skip_until_character(file, c);
}

hashmap_T *generate_defined_functions_hashmap() { 
    hashmap_T *hashmap = new_hashmap(120);
    hashmap_T *data_conversion_table = generate_data_type_conversion_table();

    FILE *file = fopen("src\\config\\defined_functions.txt", "r");
    char c;
    int param_count;
    int output_count;
    char name[20];
    char code_string[4] = "123";
    char data_type_string[20] = "";
    int code;
    defined_function_variation_T **variations = malloc(sizeof(struct DEFINED_FUNCTION_VARIATION_STRUCT *) * 15);
    int variations_count = 0;
    int *parameters;
    int *output;

    while(1) {
        if (feof(file))
            break;

        fscanf(file, "%s %d %d", name, &param_count, &output_count);
        printf("%s: params: %d, output: %d\n", name, param_count, output_count);
        variations_count = 0;
        skip_until_character(file, '(');
        for (int i = 0; i < 3; i++) {
            c = fgetc(file);
            if (c == ' ') {
                code_string[i] = '\0';
                break;
            }

            code_string[i] = c;
        }
        code = atoi(code_string);
        skip_until_character(file, '(');
        for (;;) {
            c = fgetc(file);
            if (c == '(') {
                *parameters = (int *)malloc(sizeof(int) * param_count);
                *output =  (int *)malloc(sizeof(int) * output_count);
                skip_until_character(file, '(');
                for (int i = 0; i < param_count; i++) {
                    for (int j = 0; ; j++) {
                        c = fgetc(file);
                        if (c == ' ' || c == ')') {
                            data_type_string[j] = '\0';
                            break;
                        }
                        data_type_string[j] = c;
                    }
                    printf("%s\n", data_type_string);
                    parameters[i] = *get_data_type(data_conversion_table, data_type_string);
                }
                skip_until_character(file, '(');
                for (int i = 0; i < output_count; i++) {
                    for (int j = 0; ; j++) {
                        c = fgetc(file);
                        if (c == ' ' || c == ')') {
                            data_type_string[j] = '\0';
                            break;
                        }
                        data_type_string[j] = c;
                    }
                    printf("%s\n", data_type_string);
                    output[i] = *get_data_type(data_conversion_table, data_type_string);
                }
                if (output_count == 0)
                    skip_until_character(file, ')');
                skip_until_character(file, ')');
            } else if (c == ')') {
                skip_until_character(file, ')');

                defined_function_variation_T *new_variation = malloc(sizeof(struct DEFINED_FUNCTION_VARIATION_STRUCT));
                new_variation->code = code;
                new_variation->parameters = parameters;
                new_variation->output = output;
                variations[variations_count++] = new_variation;
                break;
            }
        }

        defined_function_T *new_function = malloc(sizeof(struct DEFINED_FUNCTION_STRUCT));
        new_function->output_count = output_count;
        new_function->parameter_count = param_count;
        new_function->variation_count = variations_count;
        new_function->variations = variations;

        hashmap_set(hashmap, name, new_function);
        printf("%d", ((struct DEFINED_FUNCTION_STRUCT *)hashmap_get(hashmap, name))->variation_count);
    }

    return hashmap;
}