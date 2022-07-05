#include "pq.h"

#include "node.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

struct PriorityQueue {
    uint32_t capacity;
    uint32_t tail;
    uint32_t head;
    Node **nodes;
};

PriorityQueue *pq_create(uint32_t capacity) { //create priorityqueue
    PriorityQueue *pq = (PriorityQueue *) malloc(sizeof(PriorityQueue));
    pq->capacity = capacity; //increment one because one item is used for keeping track of tail
    pq->tail = 0;
    pq->head = 0;
    pq->nodes = (Node **) calloc(capacity, sizeof(Node **));
    if (!pq->nodes) {
        free(pq);
        pq = NULL;
    }
    return pq;
}

void pq_delete(PriorityQueue **q) { //free mem
    if ((*q) && (*q)->nodes) {
        free((*q)->nodes);
        free(*q);
        (*q) = NULL;
    }
}

bool pq_empty(PriorityQueue *q) { //is queue empty?
    if (q->tail == q->head) {
        return true;
    }
    return false;
}

bool pq_full(PriorityQueue *q) { //is queue full?
    if (q->head == (q->tail + q->capacity + 1) % q->capacity) {
        return true;
    }
    return false;
}

uint32_t pq_size(PriorityQueue *q) { //size of queue
    if (q->head > q->tail) {
        return q->tail + (q->capacity - q->head);
    } else {
        return q->tail - q->head;
    }
}

uint32_t pred(PriorityQueue *q, uint32_t num) { //previous index of queue
    return (num + q->capacity - 1) % q->capacity; //queue loops hence mod
}

bool enqueue(PriorityQueue *q, Node *n) { //enqueue to priorityqueue
    if (pq_full(q)) { //is not full
        return false;
    }
    uint32_t view = q->tail; //look at the end of queue
    q->nodes[view] = n;
    while (view != q->head && q->nodes[pred(q, view)]->frequency > n->frequency) {
        //while the freq of node you want to add is less than current view, move it left
        q->nodes[view] = q->nodes[pred(q, view)]; //swap with node at view and node you want to add
        view = pred(q, view);
    }
    q->nodes[view] = n; //ensure n is placed at the right location
    q->tail = (q->tail + 1) % q->capacity; //increase size of queue
    return true;
}

bool dequeue(PriorityQueue *q, Node **n) { //remove top item
    if (pq_empty(q)) { //queue is not empty
        return false;
    }
    *n = q->nodes[q->head]; //get node at the head
    q->head = (q->head + 1) % q->capacity; //move over head so it doesnt look at the item dequeued
    return true;
}

void pq_print(PriorityQueue *q) { //debug
    for (uint32_t i = 0; i < pq_size(q); i++) {
        printf("%c: %ld, ", q->nodes[(q->head + i) % q->capacity]->symbol,
            q->nodes[(q->head + i) % q->capacity]->frequency);
    }
    printf("\n");
}
