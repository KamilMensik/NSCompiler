#ifndef SETTINGS_H
#define SETTINGS_H

#include "list.h"

typedef struct FLAG_STRUCT {
    char *name;
    char *value;
} flag_T;

typedef struct SETTINGS_STRUCT {
    list_T *flags;
} settings_T;

#endif
