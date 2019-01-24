/*
 * Samantha Foley
 *
 * Project 4: Dining Philosophers (Common header)
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include "semaphore_support.h"


/*****************************
 * Defines
 *****************************/
#define TRUE  0
#define FALSE 1
#define THINKING 0
#define HUNGRY 1
#define EATING 2

/* 1.0 second = 1000000 usec */
//#define TIME_TO_SLEEP 100000
/* 0.5 second = 500000 usec */
//#define TIME_TO_SLEEP 50000
#define TIME_TO_SLEEP 5000


/*****************************
 * Structures
 *****************************/


/*****************************
 * Global Variables
 *****************************/
int size;
int *state;
semaphore_t mutex;
semaphore_t *chopsticks;


/*****************************
 * Function Declarations
 *****************************/
int initialize(int size);
int rand_num();
void  *dine(void *threadid);
