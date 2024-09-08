#include "include/list.h"
#include <stdlib.h>

list_T *init_list(int size) {
    list_T *list = malloc(sizeof(struct LIST_STRUCT));
    list->size = size;
    list->top = 0;
    list->array = malloc(sizeof(void *) * size);

    return list;
}

void list_push(list_T *list, void *item) {
    if (list->top == list->size) {
        list->size *= 2;
        list-> array = realloc(list->array, list->size * sizeof(void *));
    }

    list->array[list->top] = item;
    list->top += 1;
}

void *list_pop(list_T *list) {
    list->top -= 1;
    void *item = list->array[list->top];

    if (list->top < list->size / 4) {
        list->size /= 4;
        list-> array = realloc(list->array, list->size * sizeof(void *));
    }

    return item;
}
