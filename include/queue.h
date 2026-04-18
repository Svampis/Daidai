#ifndef QUEUE_H
#define QUEUE_H

#include "stddef.h"

struct Queue;

struct Queue *queue_init(void);
int queue_enqueue(struct Queue *queue, void *value);
void *queue_dequeue(struct Queue *queue);
void *queue_poll(struct Queue *queue);
int queue_is_empty(struct Queue *queue);
size_t queue_get_size(struct Queue *queue);
void queue_destroy(struct Queue *queue);

#endif
