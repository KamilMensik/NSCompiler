#ifndef STACK_H
#define STACK_H

typedef struct STACK_NODE_STRUCT stack_node_T;
struct STACK_NODE_STRUCT {
    void *item;
    stack_node_T *next;
};

typedef struct STACK_STRUCT {
    stack_node_T *top;
    unsigned int size;
} stack_T;

stack_T *init_stack();

void free_stack(stack_T *stack);

stack_node_T *init_stack_node(void *item);

void stack_push(stack_T *stack, void *item);

void *stack_pop(stack_T *stack);

void *stack_peek(stack_T *stack);

#endif
