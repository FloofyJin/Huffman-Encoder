#include "code.h"

#include "defines.h"

#include <stdint.h>
#include <stdio.h>

Code code_init(void) { //create empty Code
    Code c;
    c.top = 0;
    for (int i = 0; i < MAX_CODE_SIZE; i++) {
        c.bits[i] = 0;
    }
    return c;
}

uint32_t code_size(Code *c) { //size of code
    return c->top;
}

bool code_empty(Code *c) { //is code empty?
    if (c->top == 0) {
        return true;
    }
    return false;
}

bool code_full(Code *c) { //is code full
    if (c->top == ALPHABET) {
        return true;
    }
    return false;
}

bool code_push_bit(Code *c, uint8_t bit) { //push bit to end of code array
    if (code_full(c)) { //check code isnt full
        return false;
    }
    c->bits[c->top] = bit & 1; //add bit
    c->top++; //increment view at the top
    return true;
}

bool code_pop_bit(Code *c, uint8_t *bit) { //pop last element from code
    if (code_empty(c)) { //code isnt empty
        return false;
    }
    c->top--; //decrement the viwer
    *bit = c->bits[c->top]; //get the bit at the top
    return true;
}

void code_print(Code *c) { //debug function
    fprintf(stderr, "top: %d. code: ", c->top);
    for (uint64_t i = 0; i < c->top; i++) {
        fprintf(stderr, "%d", c->bits[i]);
        /*if (i % 16 == 15) {
            printf("\n");
        }*/
    }
    fprintf(stderr, "\n");
}
