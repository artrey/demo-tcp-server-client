#include "queue.h"
#include <stdlib.h>

queue_t* create_queue(size_t max_length)
{
	queue_t* queue = (queue_t*)malloc(sizeof(queue_t));
	if (queue == NULL)
		return NULL;

	queue->length = 0;
	queue->start = 0;
	queue->end = 0;
	queue->max_length = max_length;
	queue->arr = (value_type*)malloc(sizeof(value_type) * max_length);
	if (queue->arr == NULL)
	{
		free(queue);
		return NULL;
	}

	return queue;
}

void free_queue(queue_t* q)
{
	free(q->arr);
	free(q);
}

bool queue_enqueue(queue_t* q, value_type elem)
{
	q->arr[q->end++] = elem;
	if (q->end == q->max_length)
	{
		q->end = 0;
	}
	if (q->length == q->max_length)
	{
		if (++q->start == q->max_length)
		{
			q->start = 0;
		}
		return false;
	}
	else
	{
		++q->length;
		return true;
	}
}

bool queue_dequeue(queue_t* q, value_type* elem)
{
	if (q->length == 0)
	{
		return false;
	}
	*elem = q->arr[q->start++];
	if (q->start == q->max_length)
	{
		q->start = 0;
	}
	--q->length;
	return true;
}

bool queue_watch(queue_t* q, value_type* elem)
{
	if (q->length == 0)
	{
		return false;
	}
	*elem = q->arr[q->start];
	return true;
}

void queue_clear(queue_t* q)
{
	q->length = 0;
	q->start = 0;
	q->end = 0;
}
