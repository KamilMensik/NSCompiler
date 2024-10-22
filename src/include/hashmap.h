#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdlib.h>
#include "list.h"

typedef struct HASHMAP_NODE_STRUCT {
    char *key;
    void *item;
} hashmap_node_T;

typedef struct HASHMAP_STRUCT {
    unsigned int items_size;
    hashmap_node_T **items;
    list_T *existing_items;
} hashmap_T;

hashmap_node_T *init_hashmap_node(char *key, void *item);

hashmap_T *new_hashmap(int size);

unsigned long djb2(char *string, int size);

void hashmap_set(hashmap_T *hashmap, char *key, void *value);

void *hashmap_get(hashmap_T *hashmap, char *key);

void free_hashmap(hashmap_T *hashmap, char should_free_items);

#endif
