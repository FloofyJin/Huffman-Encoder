#include "stack.h"

#include "node.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

struct Stack {
    uint32_t top;
    uint32_t capacity;
    Node **items;
};
//stack used for treedump
Stack *stack_create(uint32_t capacity) { //create stack
    Stack *s = (Stack *) malloc(sizeof(Stack));
    if (s) {
        s->top = 0;
        s->capacity = capacity;
        s->items = (Node **) calloc(capacity, sizeof(Node *));
        if (!s->items) {
            free(s);
            s = NULL;
        }
    }
    return s;
}

void stack_delete(Stack **s) { //free mem used in stack
    if (*s && (*s)->items) {
        free((*s)->items);
        (*s)->items = NULL;
        free(*s);
        *s = NULL;
    }
}

bool stack_empty(Stack *s) { //is stack empty?
    if (s->top == 0) {
        return true;
    }
    return false;
}

bool stack_full(Stack *s) { //is stack full?
    if (s->top == s->capacity) {
        return true;
    }
    return false;
}

uint32_t stack_size(Stack *s) { //stack size
    return s->top;
}

bool stack_push(Stack *s, Node *n) { //push item at the top of stack
    if (stack_full(s)) { //stack is full, increase size. shouldnt need
        s->items = (Node **) realloc(s->items, s->capacity * 2 * sizeof(Node *));
        if (s->items == NULL) {
            return false;
        }
    }
    s->items[s->top] = n; //top item in stack
    s->top++; //increase top
    return true;
}

bool stack_pop(Stack *s, Node **n) { //pop from stack
    if (stack_empty(s)) { //stack cannot be empty
        return false;
    }
    s->top--;
    *n = s->items[s->top]; //decrease the top to get top item
    return true;
}
