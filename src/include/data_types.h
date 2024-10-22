#ifndef DATA_TYPES_H
#define DATA_TYPES_H

enum DATA_TYPES {
    NONE = 0,
    UINT = 1,
    SINT = 2,
    UBYTE = 3,
    SBYTE = 4,
    USHORT = 5,
    SSHORT = 6,
    STRING = 7,
};

extern const char data_type_strings[][16];
extern const unsigned int data_type_sizes[];

int *get_data_type(char *string);

void free_data_type_conversion_table();

#endif
