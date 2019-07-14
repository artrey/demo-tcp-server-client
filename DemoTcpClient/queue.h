#ifndef __QUEUE_QUEUE_H__
#define __QUEUE_QUEUE_H__

#include <stdint.h>
#include <stdbool.h>

typedef uint8_t value_type;

typedef struct queue
{
	value_type* arr;
	size_t length;
	size_t max_length;
	size_t start;
	size_t end;
} queue_t;

queue_t* create_queue(size_t max_length);
void free_queue(queue_t* q);

bool queue_enqueue(queue_t* q, value_type elem);
bool queue_dequeue(queue_t* q, value_type* elem);

bool queue_watch(queue_t* q, value_type* elem, size_t idx);

void queue_clear(queue_t* q);

#endif // __QUEUE_QUEUE_H__
