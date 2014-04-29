/* 
 * queue.c - linked queue
 *
 * Copyright (c) 2014 Yang Li. All rights reserved.
 *
 * This program may be distributed according to the terms of the GNU
 * General Public License, version 3 or (at your option) any later version.
 */

#include <stdlib.h>

#include "queue.h"

struct queue_node_int{
    struct queue_node_int *next;
    int data;
};

int
queue_int_enqueue(struct queue_int *q, int data){
    struct queue_node_int *node;

    if((node = malloc(sizeof(struct queue_node_int))) == NULL){
        return -1;
    }

    node->next = NULL;
    node->data = data;

    if(q->tail == NULL){
        q->head = node;
    }else{
        q->tail->next = node;
    }
    return 0;
}

int
queue_int_dequeue(struct queue_int *q){
    int data;

    if(q->head == q->tail){
        q->tail = NULL;
    }

    data = q->head->data;
    q->head = q->head->next;

    return data;
}
