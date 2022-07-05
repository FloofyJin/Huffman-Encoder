#include "huffman.h"

#include "code.h"
#include "node.h"
#include "pq.h"
#include "stack.h"

#include <stdio.h>
#include <stdlib.h>

Node *build_tree(uint64_t hist[static ALPHABET]) { //create tree of nodes
    PriorityQueue *q = pq_create(ALPHABET + 1);
    Node *n;
    for (int i = 0; i < 256; i++) { //fill priorityqueue w/ original value
        if (hist[i] > 0) {
            n = node_create(i, hist[i]);
            enqueue(q, n); //enqueue into queue
        }
    }
    while (pq_size(q) != 1) { //until only root remains, keep joining nodes
        Node *left;
        Node *right;
        dequeue(q, &left); //get left and...
        dequeue(q, &right); //get right from queue
        n = node_join(left, right); //join them
        enqueue(q, n); //put joined nodes back into queue
    }
    Node *root = NULL;
    dequeue(q, &root);
    pq_delete(&q); //free memory
    return root;
}
//creates code by visiting every leaf
static void codes(Node *root, Code table[static ALPHABET], Code c) {
    uint8_t bit;
    if (root->left != NULL) { //visit left node
        code_push_bit(&c, 0); //add 0
        codes(root->left, table, c);
        code_pop_bit(&c, &bit); //remove 0
    }
    if (root->right != NULL) { //visit right node
        code_push_bit(&c, 1); //add 1
        codes(root->right, table, c);
        code_pop_bit(&c, &bit); //remove 1
    }
    if (root->left == NULL && root->right == NULL) { //is a leaf
        table[root->symbol] = c; //add to table
    }
}

void build_codes(Node *root, Code table[static ALPHABET]) { //populates code table
    Code c = code_init(); //tracks current code
    codes(root, table, c); //call codes function above
}
//join nodes from treedump
Node *rebuild_tree(uint16_t nbytes, uint8_t tree_dump[static nbytes]) {
    Stack *s = stack_create(nbytes);
    for (int i = 0; i < nbytes; i++) { //read every byte
        if (tree_dump[i] == 'L') { //is a leaf
            Node *n = node_create(tree_dump[i + 1], 0);
            stack_push(s, n);
            i++;
        } else { //is internal branch
            Node *right;
            Node *left;
            stack_pop(s, &right); //take out right and...
            stack_pop(s, &left); //take out left from stack
            Node *combined = node_join(left, right); //join them
            stack_push(s, combined); //put it back into stack
        }
    }
    if (stack_size(s) == 1) { //only root remains in stack
        Node *root;
        stack_pop(s, &root); //get the root
        stack_delete(&s); //free memory
        return root; //return the root of the constructed tree
    } else { //no root. normally should not happen
        stack_delete(&s);
        fprintf(stderr, "no root found\n");
        return NULL;
    }
}

void delete_tree(Node **root) { //free memory
    if ((*root)->left) { //delete everything on the left
        delete_tree(&(*root)->left);
    }
    if ((*root)->right) { //delete everything on the right
        delete_tree(&(*root)->right);
    }
    node_delete(root); //recurse down the tree
}
