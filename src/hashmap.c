#include "include/hashmap.h"
#include "include/list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

hashmap_node_T *init_hashmap_node(char *key, void *item) {
    hashmap_node_T *new_node = malloc(sizeof(struct HASHMAP_NODE_STRUCT));
    new_node->key = key;
    new_node->item = item;

    return new_node;
}

hashmap_T *new_hashmap(int size) {
    hashmap_T *new_hashmap = malloc(sizeof(struct HASHMAP_STRUCT));
    new_hashmap->items_size = size;
    new_hashmap->items = calloc(size, sizeof(struct HASHMAP_NODE_STRUCT *));
    new_hashmap->existing_items = init_list(8);

    return new_hashmap;
}

unsigned long djb2(char *string, int size) {
    unsigned long key = 5381;
    int c;

    while ((c = *string)) {
        key = key * 33 + c;
        string += 1;
    }
        

    return key % size;
}

void hashmap_set(hashmap_T *hashmap, char *key, void *value) {
    int hashed_key = djb2(key, hashmap->items_size);

    for (int i = 0; ; i ++) {
        int iterated_key = (hashed_key + i * i) % hashmap->items_size;
        if (hashmap->items[iterated_key] == NULL) {
            char *kept_key = malloc(sizeof(char) * (strlen(key) + 1));
            strcpy(kept_key, key);
            hashmap_node_T *new_node = init_hashmap_node(kept_key, value);
            list_push(hashmap->existing_items, kept_key);
            hashmap->items[iterated_key] = new_node;
            break;
        }
    }
}

hashmap_node_T **get_node(hashmap_T *hashmap, char *key, char can_be_null) {
    int hashed_key = djb2(key, hashmap->items_size);

    for (int i = 0;; i ++) {
        int iterated_key = (hashed_key + i * i) % hashmap->items_size;
        if (hashmap->items[iterated_key] == NULL) {
            if (can_be_null)
                return NULL;
        } else {
            if (strcmp(hashmap->items[iterated_key]->key, key) == 0)
                return &hashmap->items[iterated_key];
        }
    }
}

void *hashmap_get(hashmap_T *hashmap, char *key) {
    hashmap_node_T **node = get_node(hashmap, key, 1);
    return node ? (*node)->item : NULL;
}

void free_hashmap(hashmap_T *hashmap, char should_free_items) {
    for (int i = 0; i < hashmap->existing_items->top; i++) {
        char *key = (char *)(hashmap->existing_items->array[i]);
        hashmap_node_T **node_pos = get_node(hashmap, key, 0);
        hashmap_node_T *node = *node_pos;
        *node_pos = NULL;
        if (should_free_items)
            free(node->item);
        free(node);
        free(key);
    }
    free(hashmap->items);
    free_list(hashmap->existing_items);
    free(hashmap);
}
