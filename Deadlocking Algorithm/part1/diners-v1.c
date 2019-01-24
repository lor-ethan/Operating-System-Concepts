/*
 * Samantha Foley, Ethan Lor, Tyler Durr
 *
 * Project 4: Dining Philosophers
 * Deadlocking Algorithm
 *
 */
#include "diners.h"

void *printHello(void *threadid) {
  int tid = (intptr_t)threadid;
  printf("Hello World! It's me, thread #%d!\n", tid);
  pthread_exit(NULL);
}

int main(int argc, char * argv[]) {
    int ret, i;

    if (argc < 3) {
      printf("Usage: %s <positive integer greater than 0> <positive integer greater than 1>\n", argv[0]);
      exit(-1);
    }

    size = atoi(argv[2]);
    pthread_t threads[size];

    // Initialize size according to number of philosophers
    if( 0 != (ret = initialize(size)) ) {
        fprintf(stderr, "Error: initialize() failed with %d\n", ret);
        return -1;
    }

    for(i = 0; i < size; ++i) {
        printf("In Main(): Creating thread %d\n", i);
        ret = pthread_create(&threads[i], NULL, dine, (void *)(intptr_t)i);
        if(0 != ret ) {
            fprintf(stderr, "Error: Cannot Create thread\n");
            exit(-1);
        }
    }

    for(i = 0; i < size; i++){
      pthread_join(threads[i], NULL);
      printf("Thread %d done\n", i);
    }

    printf("HERE in main!");

    return 0;
}

// Initialize all semaphores according to the size/number of philosophers
int initialize (int size) {
    int i;

    if( 0 != semaphore_create(&mutex, 1) ) {
      return -1;
    }
    if( NULL == (state = (int*) calloc(size, sizeof(int))) ) {
      return -1;
    }
    if( NULL == (chopsticks = (semaphore_t*) calloc(size, sizeof(semaphore_t))) ) {
      return -1;
    }
    for(i = 0; i < size; i++) {
      if( 0 != semaphore_create(&chopsticks[i], 1) ) {
        return -1;
      }
    }

    return 0;
}

int rand_num() {
    srand ( time(NULL) );
    return rand() % 5001;
}

void *dine(void *threadid) {
  int tid = (intptr_t)threadid;
  int j = 0;
  while(j < 5) {
    // Think
    printf("%d thinking\n", tid);
    semaphore_wait(&chopsticks[tid]);
    semaphore_wait(&chopsticks[(tid + 1) % size]);
    // Eat
    printf("%d eating\n", tid);
    semaphore_post(&chopsticks[tid]);
    semaphore_post(&chopsticks[(tid + 1) % size]);
    j++;
  }
  pthread_exit(NULL);
}
