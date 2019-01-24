/*
 *
 *
 * CS 441/541: Queue (Project 5)
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "semaphore_support.h"

/*****************************
 * Defines
 *****************************/


/*****************************
 * Structures
 *****************************/
struct Node {
	struct Node *after;
	semaphore_t *voter_sem;
};

struct Queue {
	struct Node *front;
	struct Node *end;
	int size;
};

/*****************************
 * Global Variables
 *****************************/


/*****************************
 * Function Declarations
 *****************************/
struct Queue *queueCreate();
void queueDestroy(struct Queue *q);
void queuePush(struct Queue *q, semaphore_t *sem);
semaphore_t *queuePeek(struct Queue *q);
semaphore_t *queuePop(struct Queue *q);
void queueEmpty(struct Queue *q);
int queueSize(struct Queue *q);
