/*
 *
 *
 * CS 441/541: Finicky Voter (Project 5)
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include "semaphore_support.h"

/*****************************
 * Defines
 *****************************/
#define MAX_SIGNIN_TIME 50001
#define MAX_VOTE_TIME 100001

/*****************************
 * Global Variables
 *****************************/

semaphore_t dem_mutex;
semaphore_t dems;
semaphore_t rep_mutex;
semaphore_t reps;
semaphore_t empty;
semaphore_t barrier;
semaphore_t booth_mutex;
semaphore_t party_in_booths_mutex;
semaphore_t line_queue_mutex;
semaphore_t booth_queue_mutex;
semaphore_t print_mutex;
int dems_in_voting_area = 0;
int reps_in_voting_area= 0;
enum party_enum {REP = 1, DEM = 2, IND = 0};
enum party_enum party_in_booths = 0;
int empty_booths;
pthread_t *rep_threads;
pthread_t *dem_threads;
pthread_t *ind_threads;
struct Queue *booth_queue;
struct Queue *line_queue;
int booth_count;
char *booths;

/*****************************
 * Structures
 *****************************/
struct voter_info {
	enum party_enum party;
	int id;
	semaphore_t my_sem;
};
typedef struct voter_info voter_info;

/*****************************
 * Function Declarations
 *****************************/

int init(int rep_count, int dem_count, int ind_count);
int createBooths(int count);
int createReps(int count);
int createDems(int count);
int createInds(int count);
void print_divider();
void print_status(char *party, int id, int booth, char *status);
void *vote(void *arg);
int find_booth(char party);
void leave_booth(int booth_id);
int voteRep(voter_info *my_info);
int voteDem(voter_info *my_info);
int voteInd(voter_info *my_info);
int is_num(char *num_str);
int parse_args(int argc,
			   								char *argv[],
	       								int *rep_count,
			   								int *dem_count,
			   								int *ind_count);
