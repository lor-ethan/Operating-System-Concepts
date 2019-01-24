/*
 *
 *
 * CS 441/541: Queue (Project 5)
 *
 */
#include "queue.h"

struct Queue *queueCreate() {
	struct Queue *newQueue = malloc(sizeof(struct Queue));
	newQueue -> size = 0;
	return newQueue;
}

void queueDestroy(struct Queue *q) {
	queueEmpty(q);
	free(q);
}

void queuePush(struct Queue *q, semaphore_t *sem) {
	struct Node *newNode = malloc(sizeof(struct Node));
	newNode -> after = NULL;
	newNode -> voter_sem = sem;
	if( queueSize(q) == 0 ) {
		q -> front = newNode;
	}
	else {
		q -> end -> after = newNode;
	}
	q -> end = newNode;
	q -> size++;
}

semaphore_t *queuePop(struct Queue *q) {
	struct Node *next = q -> front;
	semaphore_t *next_sem = next -> voter_sem;
	if( queueSize(q) != 0 ) {
		q -> front = next -> after;
		if( queueSize(q) == 1 ) {
			q -> end = NULL;
		}
		q -> size--;
	}
	free(next);
	return next_sem;
}

semaphore_t *queuePeek(struct Queue *q) {
	if( queueSize(q) != 0 ) {
		return q -> front -> voter_sem;
	}

	return NULL;
}

void queueEmpty(struct Queue *q) {
	while( queueSize(q) != 0 ) {
		queuePop(q);
	}
}

int queueSize(struct Queue *q) {
	return q -> size;
}
