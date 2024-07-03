#include "include/defined_functions.h"
#include <stdarg.h>
#include <stdio.h>

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
    if (fgetc(file) == c || feof(file)) return;

    skip_until_character(file, c);
}

void generate_defined_functions_hashmap() { 
    hashmap_T *hashmap = new_hashmap(120);
    hashmap_T *data_conversion_table = generate_data_type_conversion_table();

    FILE *file = fopen("src\\config\\defined_functions.cfg", "r");
    char c;
    int param_count;
    int output_count;
    char name[20];
    char code_string[4] = "123";
    char data_type_string[20] = "";
    int code;
    defined_function_variation_T **variations = malloc(sizeof(struct DEFINED_FUNCTION_VARIATION_STRUCT *) * 15);
    int variations_count = 0;
    int *parameters_tmp = malloc(sizeof(int) * 20);
    int *output_tmp = malloc(sizeof(int) * 20);

    while(1) {
        if (feof(file))
            break;

        fscanf(file, "%s %d %d", name, &param_count, &output_count);
        variations_count = 0;
        skip_until_character(file, '(');
        for (;;) {
            c = fgetc(file);
            if (c == '(') {
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
                for (int i = 0; i < param_count; i++) {
                    for (int j = 0; ; j++) {
                        c = fgetc(file);
                        if (c == ' ' || c == ')') {
                            data_type_string[j] = '\0';
                            break;
                        }
                        data_type_string[j] = c;
                    }
                    parameters_tmp[i] = *get_data_type(data_conversion_table, data_type_string);
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
                    output_tmp[i] = *get_data_type(data_conversion_table, data_type_string);
                }

                if (output_count == 0)
                    skip_until_character(file, ')');
                skip_until_character(file, ')');
            } else if (c == ')') {
                defined_function_variation_T *new_variation = malloc(sizeof(struct DEFINED_FUNCTION_VARIATION_STRUCT));
                new_variation->code = code;
                new_variation->parameters = malloc(sizeof(int) * param_count);
                for (int i = 0; i < param_count; i++)
                    new_variation->parameters[i] = parameters_tmp[i];
                new_variation->output = malloc(sizeof(int) * output_count);
                for (int i = 0; i < output_count; i++)
                    new_variation->output[i] = output_tmp[i];
                variations[variations_count++] = new_variation;
                skip_until_character(file, '\n');
                break;
            }
        }

        defined_function_variation_T **kept_variations = malloc(sizeof(struct DEFINED_FUNCTION_VARIATION_STRUCT *) * variations_count);
        for (int i = 0; i < variations_count; i++) {
            kept_variations[i] = variations[i];
        }
        defined_function_T *new_function = malloc(sizeof(struct DEFINED_FUNCTION_STRUCT));
        new_function->output_count = output_count;
        new_function->parameter_count = param_count;
        new_function->variation_count = variations_count;
        new_function->variations = kept_variations;
        hashmap_set(hashmap, name, new_function);
    }
    
    defined_functions_hashmap = hashmap;
}

void defined_functions_hashmap_set(char *key, defined_function_T *defined_function) {
    hashmap_set(defined_functions_hashmap, key, defined_function);
}

defined_function_T *defined_functions_hashmap_get(char *key) {
    return (defined_function_T *)hashmap_get(defined_functions_hashmap, key);
}