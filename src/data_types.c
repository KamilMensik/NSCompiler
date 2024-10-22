#include "include/data_types.h"
#include "include/hashmap.h"
#include "stdio.h"
#include <string.h>

const char data_type_strings[][16] = { "NONE", "UINT", "INT", "UBYTE", "BYTE", "USHORT", "SHORT", };
const unsigned data_type_sizes[] = { 0, 4, 4, 1, 1, 2, 2, };

unsigned int data_type_strings_size = (sizeof(data_type_strings) / sizeof(*data_type_strings));
hashmap_T *data_type_conversion_table = NULL;

hashmap_T *generate_data_type_conversion_table() {
    hashmap_T *hashmap = new_hashmap(20);

    for (int i = 0; i < data_type_strings_size; i++) {
        int *data_type = malloc(sizeof(int) * 2);
        char copied_string[16] = {};
        strcpy(copied_string, data_type_strings[i]);

        data_type[0] = i;
        data_type[1] = data_type_sizes[i];
        hashmap_set(hashmap, copied_string, data_type);
    }

    return hashmap;
}

int *get_data_type(char *string) {
    if (!data_type_conversion_table)
        data_type_conversion_table = generate_data_type_conversion_table();

    int *res = hashmap_get(data_type_conversion_table, string);
    return res;
}
