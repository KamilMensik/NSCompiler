#ifndef LIST_H
#define LIST_H

typedef struct LIST_STRUCT list_T;

#include "token.h"

struct LIST_STRUCT {
    unsigned long top;
    unsigned long size;
    void **array;
};

list_T *init_list(int size);

void free_list(list_T *list);

void list_push(list_T *list, void *item);

void *list_pop(list_T *list);

#endif
