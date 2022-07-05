#include "node.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

Node *node_create(uint8_t symbol, uint64_t frequency) { //create node
    Node *n = (Node *) malloc(sizeof(Node));
    n->symbol = symbol;
    n->frequency = frequency;
    n->left = NULL;
    n->right = NULL;
    return n;
}

void node_delete(Node **n) { //delete and free node
    if ((*n) != NULL) {
        free(*n);
        *n = NULL;
    }
}

Node *node_join(Node *left, Node *right) { //join left and right nodes
    Node *n = node_create('$', left->frequency + right->frequency);
    n->left = left; //create nodes and set left and right
    n->right = right;
    return n;
}

void node_print(Node *n) { //debug
    if (n->left != NULL && n->right != NULL) {
        fprintf(
            stderr, "char:%c, left:%c, right:%c\n", n->symbol, n->left->symbol, n->right->symbol);
        //node_print(n->left);
        //node_print(n->right);
    }
}
