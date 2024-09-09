#include "include/stack.h"
#include <stdlib.h>

stack_T *init_stack() {
    stack_T *stack = calloc(1, sizeof(struct STACK_STRUCT));
    stack->top = NULL;
    stack->size = 0;

    return stack;
}

stack_node_T *init_stack_node(void *item) {
    stack_node_T *node = calloc(1, sizeof(struct STACK_NODE_STRUCT));
    node->item = item;

    return node;
}

void stack_push(stack_T *stack, void *item) {
    stack_node_T *new_node = init_stack_node(item);
    new_node->next = stack->top;
    stack->top = new_node;
    stack->size += 1;
}

void *stack_pop(stack_T *stack) {
    if (stack->size == 0)
        return NULL;

    stack_node_T *stack_node = stack->top;
    stack->top = stack_node->next;
    void *item = stack_node->item;
    free(stack_node);
    stack->size -= 1;
    return item;
}

void *stack_peek(stack_T *stack) {
    if (stack->size == 0)
        return NULL;

    return stack->top->item;
}
