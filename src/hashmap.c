#include "include/hashmap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

hashmap_node_T *init_hashmap_node(char *key, void *item) {
    hashmap_node_T *new_node = malloc(sizeof(struct HASHMAP_NODE_STRUCT));
    new_node->key = key;
    new_node->item = item;
}

hashmap_T *new_hashmap(int size) {
    hashmap_T *new_hashmap = malloc(sizeof(struct HASHMAP_STRUCT));
    new_hashmap->items_size = size;
    new_hashmap->items = calloc(size, sizeof(struct HASHMAP_NODE_STRUCT *));

    return new_hashmap;
}

unsigned long djb2(char *string, int size) {
    unsigned long key = 5381;
    int c;

    while ((c = *string++))
        key = key * 33 + c;

    return key % size;
}

void hashmap_set(hashmap_T *hashmap, char *key, void *value) {
    int hashed_key = djb2(key, hashmap->items_size);

    for (int i = 0; ; i ++) {
        int iterated_key = (hashed_key + i * i) % hashmap->items_size;
        if (hashmap->items[iterated_key] == NULL) {
            hashmap_node_T *new_node = init_hashmap_node(key, value);
            hashmap->items[iterated_key] = new_node;
            break;
        }
    }
}

void *hashmap_get(hashmap_T *hashmap, char *key) {
    int hashed_key = djb2(key, hashmap->items_size);

    for (int i = 0; ; i ++) {
        int iterated_key = (hashed_key + i * i) % hashmap->items_size;
        if (hashmap->items[iterated_key] == NULL) {
            printf("Key %s not found in hashmap!", key);
            return NULL;
        }

        if (strcmp(hashmap->items[iterated_key]->key, key) == 0)
            return hashmap->items[iterated_key]->item;
    }
}