#include "queue.h"
#include <stdlib.h>

struct Queue
{
	struct Node *head;
	struct Node *tail;
	size_t size;
};
struct Node
{
	struct Node *next;
	void *value;
};

struct Queue *queue_init(void)
{
	struct Queue *queue = malloc(sizeof(struct Queue));
	if(queue == NULL)
	{
		return NULL;
	}
	queue->head = queue->tail = NULL;
	queue->size = 0;
	return queue;
}

int queue_enqueue(struct Queue *queue, void *value)
{
	struct Node *new = malloc(sizeof(struct Node));
	if(new == NULL)
	{
		return -1;
	}
	new->next = NULL;
	new->value = value;
	
	if(queue->tail == NULL)
	{
		queue->head = queue->tail = new;
	}
	else
	{
		queue->tail->next = new;
		queue->tail = new;
	}
	queue->size++;
	return 0;
}

void *queue_dequeue(struct Queue *queue)
{
	void *out;
	struct Node *temp;
	
	if(queue->tail == NULL)
	{
		return NULL;
	}

	temp = queue->head;
	out = temp->value;
	queue->head = queue->head->next;

	if(queue->head == NULL)
	{
		queue->tail = NULL;
	}

	free(temp);
	queue->size--;
	return out;
}

int queue_is_empty(struct Queue *queue)
{
	return queue->head == NULL;
}

void queue_destroy(struct Queue *queue)
{
	while(!queue_is_empty(queue))
	{
		queue_dequeue(queue);
	}
	free(queue);
}

void *queue_poll(struct Queue *queue)
{
	if(queue->head == NULL)
	{
		return NULL;
	}
	return queue->head->value;
}

size_t queue_get_size(struct Queue *queue)
{
	return queue->size;
}
