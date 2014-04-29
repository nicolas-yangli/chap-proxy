/* 
 * queue.h - linked queue
 *
 * Copyright (c) 2014 Yang Li. All rights reserved.
 *
 * This program may be distributed according to the terms of the GNU
 * General Public License, version 3 or (at your option) any later version.
 */

#ifndef QUEUE_H
#define QUEUE_H

struct queue_node_int;

struct queue_int{
    struct queue_node_int *head;
    struct queue_node_int *tail;
};

#define queue_init(q)       ((q)->tail = (q)->head = NULL)
#define queue_isempty(q)    ((q)->head == NULL)

int queue_int_enqueue(struct queue_int *q, int data);
int queue_int_dequeue(struct queue_int *q);

#endif  /*  QUEUE_H */
