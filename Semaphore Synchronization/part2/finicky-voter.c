/*
 * CS 441/541: Finicky Voter (Project 5)
 * Tyler Durr & Ethan Lor
 *
 */
#include "finicky-voter.h"
#include "queue.c"

int init(int rep_count, int dem_count, int ind_count) {

	int ret;
	int i = 0;
	srand(time(NULL));

	booths = malloc(sizeof(char) * booth_count);
	for( i = 0; i < booth_count; i++ ) {
		booths[i] = '.';
	}

	// Create semaphores
	if( 0 != (ret = semaphore_create(&print_mutex, 1)) ) {
		fprintf(stderr, "Error: semaphore_create() failed with %d\n", ret);
		return -1;
	}

	if( 0 != (ret = semaphore_create(&booth_mutex, 1)) ) {
		fprintf(stderr, "Error: semaphore_create() failed with %d\n", ret);
		return -1;
	}

	if( 0 != (ret = semaphore_create(&booth_queue_mutex, 1)) ) {
		fprintf(stderr, "Error: semaphore_create() failed with %d\n", ret);
		return -1;
	}

	if( 0 != (ret = semaphore_create(&line_queue_mutex, 1)) ) {
		fprintf(stderr, "Error: semaphore_create() failed with %d\n", ret);
		return -1;
	}

	if( 0 != (ret = semaphore_create(&party_in_booths_mutex, 1)) ) {
		fprintf(stderr, "Error: semaphore_create() failed with %d\n", ret);
		return -1;
	}

	if( 0 != (ret = semaphore_create(&dem_mutex, 1)) ) {
		fprintf(stderr, "Error: semaphore_create() failed with %d\n", ret);
		return -1;
	}

	if( 0 != (ret = semaphore_create(&dems, booth_count)) ) {
		fprintf(stderr, "Error: semaphore_create() failed with %d\n", ret);
		return -1;
	}

	if( 0 != (ret = semaphore_create(&rep_mutex, 1)) ) {
		fprintf(stderr, "Error: semaphore_create() failed with %d\n", ret);
		return -1;
	}

	if( 0 != (ret = semaphore_create(&reps, booth_count)) ) {
		fprintf(stderr, "Error: semaphore_create() failed with %d\n", ret);
		return -1;
	}

	if( 0 != (ret = semaphore_create(&empty, 1)) ) {
		fprintf(stderr, "Error: semaphore_create() failed with %d\n", ret);
		return -1;
	}

	// Initialize barrier
	if( 0 != (ret = semaphore_create(&barrier, 0)) ) {
		fprintf(stderr, "Error %i: semaphore_create() failed for barrier with\n", ret);
		return -1;
	}

	// Initialize queues
	if( !(booth_queue = queueCreate()) ) {
		fprintf(stderr, "Error %i: Failed to create booth_queue\n", ret);
		return -1;
	}

	if( !(line_queue = queueCreate()) ) {
		fprintf(stderr, "Error %i: Failed to create line_queue\n", ret);
		return -1;
	}

	print_divider();

	// Create voters
	createBooths(booth_count);
	createReps(rep_count);
	createDems(dem_count);
	createInds(ind_count);

	return 0;
}

int destroy(int rep_count, int dem_count, int ind_count) {

	// Destroy voters
	free(booths);
	free(rep_threads);
	free(dem_threads);
	free(ind_threads);

	// Destroy queues
	queueDestroy(booth_queue);
	queueDestroy(line_queue);

	// Destroy semaphores
	semaphore_destroy(&print_mutex);
	semaphore_destroy(&booth_mutex);
	semaphore_destroy(&booth_queue_mutex);
	semaphore_destroy(&line_queue_mutex);
	semaphore_destroy(&party_in_booths_mutex);
	semaphore_destroy(&dem_mutex);
	semaphore_destroy(&dems);
	semaphore_destroy(&rep_mutex);
	semaphore_destroy(&reps);
	semaphore_destroy(&empty);
	semaphore_destroy(&barrier);

	return 0;
}

int createBooths(int count) {

	empty_booths = count;

	return 0;
}

int createReps(int count) {

	int ret = 0;
	int i = 0;
	rep_threads = malloc(sizeof(pthread_t) * count);

	for( i = 0; i < count; i++ ) {
		voter_info *info = malloc(sizeof(voter_info));
		info -> party = REP;
		info -> id = i;
		semaphore_create(&info->my_sem, 0);
		ret = pthread_create(&rep_threads[i], NULL, vote, (void *) info);
		if( ret != 0 ) {
			fprintf(stderr, "Failed to create republican %i.\n", i);
			return -1;
		}
	}

	return 0;
}

int createDems(int count) {

	int ret = 0;
	int i = 0;
	dem_threads = malloc(sizeof(pthread_t) * count);

	for( i = 0; i < count; i++ ) {
		voter_info *info = malloc(sizeof(voter_info));
		info -> party = DEM;
		info -> id = i;
		semaphore_create(&info->my_sem, 0);
		ret = pthread_create(&dem_threads[i], NULL, vote, (void *) info);
		if( ret != 0 ) {
			fprintf(stderr, "Failed to create democrat %i.\n", i);
			return -1;
		}
	}

	return 0;
}

int createInds(int count) {

	int ret = 0;
	int i = 0;
	ind_threads = malloc(sizeof(pthread_t) * count);

	for( i = 0; i < count; i++ ) {
		voter_info *info = malloc(sizeof(voter_info));
		info -> party = IND;
		info -> id = i;
		semaphore_create(&info->my_sem, 0);
		ret = pthread_create(&ind_threads[i], NULL, vote, (void *) info);
		if( ret != 0 ) {
			fprintf(stderr, "Failed to create independent %i.\n", i);
			return -1;
		}
	}

	return 0;
}

void print_divider() {
	int i = 0;
	semaphore_wait(&print_mutex);
	printf("-----------------------------+------");
	for( i = 0; i < booth_count; i++ ) {
		printf("---");
	}
	printf("+--------------------------------\n");
	fflush(NULL);
	semaphore_post(&print_mutex);
}

void print_status(char *party, int id, int booth, char *status) {
	int i = 0;

	semaphore_wait(&print_mutex);
	printf("%s\t%5d", party, id);
	if( booth >= 0 ) {
		printf(" in %3d ", booth);
	}
	else {
		printf("\t     ");
	}
	printf("|-> ");
	for( i = 0; i < booth_count; i++ ) {
		printf("[%c]", booths[i]);
	}
	printf(" <-| %s\n", status);
	fflush(NULL);
	semaphore_post(&print_mutex);
}

void *vote(void *arg) {
	voter_info *my_info = (voter_info *) arg;

	if( my_info -> party == REP ) {
		print_status("Republican", my_info -> id, -1, "Waiting for polling station to open...");
		semaphore_wait(&barrier);
		print_status("Republican", my_info -> id, -1, "Entering the polling station");
		voteRep(my_info);
	}
	else if( my_info -> party == DEM ) {
		print_status("Democrat", my_info -> id, -1, "Waiting for polling station to open...");
		semaphore_wait(&barrier);
		print_status("Democrat", my_info -> id, -1, "Entering the polling station");
		voteDem(my_info);
	}
	else {
		print_status("Independent", my_info -> id, -1, "Waiting for polling station to open...");
		semaphore_wait(&barrier);
		print_status("Independent", my_info -> id, -1, "Entering the polling station");
		voteInd(my_info);
	}

	semaphore_destroy(&my_info -> my_sem);
	free(my_info);
	pthread_exit(NULL);
}

int find_booth(char party) {
	int i = 0;

	while( booths[i] != '.' ) {
		i++;
	}
	booths[i] = party;
	empty_booths--;

	return i;
}

void leave_booth(int booth_id) {
	booths[booth_id] = '.';
	empty_booths++;
}

int voteRep(voter_info *my_info) {

	int my_booth = -1;

	// Sign in for 0–50,000 microseconds
	usleep(rand() % MAX_SIGNIN_TIME);

	// Pass your own semaphore into line queue
	// (Wait in line after registering)
	semaphore_wait(&line_queue_mutex);
	queuePush(line_queue, &my_info->my_sem);

	// If there's a line or the democrats are voting, wait in line
	semaphore_wait(&party_in_booths_mutex);
	if( queueSize(line_queue) > 1 || party_in_booths == DEM ) {
		semaphore_post(&party_in_booths_mutex);
		semaphore_post(&line_queue_mutex);

		// Wait in line while your party isn't voting (the voter in front of you
	    // in line will wake you up so you can check whether you can move to the
	    // booth queue)
		do {
			semaphore_wait(&my_info->my_sem);
		} while( party_in_booths == DEM );
		party_in_booths = REP;

		// Awaken the next voter so they can check whether they can move to the
	    // booth queue
		semaphore_wait(&line_queue_mutex);
		queuePop(line_queue);
		if( queueSize(line_queue) != 0 ) {
			semaphore_post(queuePeek(line_queue));
		}
		semaphore_post(&line_queue_mutex);
		semaphore_wait(&party_in_booths_mutex);
	}

	// If you're first in line and no democrats are voting, leave the line
	else {
		queuePop(line_queue);
		semaphore_post(&line_queue_mutex);
	}

	party_in_booths = REP;
	semaphore_post(&party_in_booths_mutex);

	// Add yourself to the count of people from your party currently voting
	// (doesn't include independents)
	semaphore_wait(&rep_mutex);
	reps_in_voting_area++;
	semaphore_post(&rep_mutex);

	// Pass your own semaphore into booth queue
	// (Wait in line for a booth)
	semaphore_wait(&booth_queue_mutex);
	queuePush(booth_queue, &my_info->my_sem);

	// If there's a line or there are no open booths, wait in line
	print_status("Republican", my_info->id, my_booth, "Waiting on a voting booth");
	party_in_booths = REP;
	semaphore_wait(&booth_mutex);
	if( queueSize(booth_queue) > 1 || empty_booths <= 0 ) {
		semaphore_post(&booth_mutex);
		semaphore_post(&booth_queue_mutex);
		semaphore_wait(&my_info->my_sem);
		semaphore_wait(&booth_mutex);
	}

	// If you're first in line and there's an empty booth, leave the line
	else {
		queuePop(booth_queue);
		semaphore_post(&booth_queue_mutex);
	}

	// Take a booth
	my_booth = find_booth('R');
	semaphore_post(&booth_mutex);

	// Vote (i.e., sleep for 0–100,000 microseconds)
	print_status("Republican", my_info->id, my_booth, "Voting!");
	usleep(rand() % MAX_VOTE_TIME);

	// Leave your booth
	semaphore_wait(&booth_mutex);
	leave_booth(my_booth);
	semaphore_post(&booth_mutex);
	print_status("Republican", my_info->id, -1, "Leaving the polling station");

	// Let the next person enter the booths
	semaphore_wait(&booth_queue_mutex);
	if( queueSize(booth_queue) != 0 ) {
		semaphore_post(queuePop(booth_queue));
	}
	semaphore_post(&booth_queue_mutex);

	// If you are the last person from your party voting, signal the line queue
	// to move ahead (let the other party start to vote)
	semaphore_wait(&rep_mutex);
	if( --reps_in_voting_area == 0 ) {
		party_in_booths = 0;
		semaphore_wait(&line_queue_mutex);
		if( queueSize(line_queue) != 0 ) {
			semaphore_post(queuePeek(line_queue));
		}
		semaphore_post(&line_queue_mutex);
	}
	semaphore_post(&rep_mutex);

  return 0;
}

int voteDem(voter_info *my_info) {

	int my_booth = -1;

	// Sign in for 0–50,000 microseconds
	usleep(rand() % MAX_SIGNIN_TIME);

	// Pass your own semaphore into line queue
	// (Wait in line after registering)
	semaphore_wait(&line_queue_mutex);
	queuePush(line_queue, &my_info->my_sem);

	// If there's a line or the republicans are voting, wait in line
	semaphore_wait(&party_in_booths_mutex);
	if( queueSize(line_queue) > 1 || party_in_booths == REP ) {
		semaphore_post(&party_in_booths_mutex);
		semaphore_post(&line_queue_mutex);

		// Wait in line while your party isn't voting (the voter in front of you
	    // in line will wake you up so you can check whether you can move to the
	    // booth queue)
		do {
			semaphore_wait(&my_info->my_sem);
		} while( party_in_booths == REP );
		party_in_booths = DEM;

		// Awaken the next voter so they can check whether they can move to the
	    // booth queue)
		semaphore_wait(&line_queue_mutex);
		queuePop(line_queue);
		if( queueSize(line_queue) != 0 ) {
			semaphore_post(queuePeek(line_queue));
		}
		semaphore_post(&line_queue_mutex);
		semaphore_wait(&party_in_booths_mutex);
	}

	// If you're first in line and no republicans are voting, leave the line
	else {
		queuePop(line_queue);
		semaphore_post(&line_queue_mutex);
	}

	party_in_booths = DEM;
	semaphore_post(&party_in_booths_mutex);

	// Add yourself to the count of people from your party currently voting
	// (doesn't include independents)
	semaphore_wait(&dem_mutex);
	dems_in_voting_area++;
	semaphore_post(&dem_mutex);

	// Pass your own semaphore into booth queue
	// (Wait in line for a booth)
	semaphore_wait(&booth_queue_mutex);
	queuePush(booth_queue, &my_info->my_sem);

	// If there's a line or there are no open booths, wait in line
	print_status("Democrat", my_info->id, my_booth, "Waiting on a voting booth");
	party_in_booths = DEM;
	semaphore_wait(&booth_mutex);
	if( queueSize(booth_queue) > 1 || empty_booths <= 0 ) {
		semaphore_post(&booth_mutex);
		semaphore_post(&booth_queue_mutex);
		semaphore_wait(&my_info->my_sem);
		semaphore_wait(&booth_mutex);
	}

	// If you're first in line and there's an empty booth, leave the line
	else {
		queuePop(booth_queue);
		semaphore_post(&booth_queue_mutex);
	}

	// Take a booth
	my_booth = find_booth('D');
	semaphore_post(&booth_mutex);

	// Vote (i.e., sleep for 0–100,000 microseconds)
	print_status("Democrat", my_info->id, my_booth, "Voting!");
	usleep(rand() % MAX_VOTE_TIME);

	// Leave your booth
	semaphore_wait(&booth_mutex);
	leave_booth(my_booth);
	semaphore_post(&booth_mutex);
	print_status("Democrat", my_info->id, -1, "Leaving the polling station");

	// Let the next person enter the booths
	semaphore_wait(&booth_queue_mutex);
	if( queueSize(booth_queue) != 0 ) {
		semaphore_post(queuePop(booth_queue));
	}
	semaphore_post(&booth_queue_mutex);

	// If you are the last person from your party voting, signal the line queue
	// to move ahead (let the other party start to vote)
	semaphore_wait(&dem_mutex);
	if( --dems_in_voting_area == 0 ) {
		party_in_booths = 0;
		semaphore_wait(&line_queue_mutex);
		if( queueSize(line_queue) != 0 ) {
			semaphore_post(queuePeek(line_queue));
		}
		semaphore_post(&line_queue_mutex);
	}
	semaphore_post(&dem_mutex);

  return 0;
}

int voteInd(voter_info *my_info) {

	int my_booth = -1;

	// Sign in for 0–50,000 microseconds
	usleep(rand() % MAX_SIGNIN_TIME);

	// Pass your own semaphore into line queue
	// (Wait in line after registering)
	semaphore_wait(&line_queue_mutex);
	queuePush(line_queue, &my_info->my_sem);

	// If there's a line, wait in line
	if( queueSize(line_queue) > 1 ) {
		semaphore_post(&line_queue_mutex);

		// Wait in line
		semaphore_wait(&my_info->my_sem);

		// Awaken the next voter so they can check whether they can move to the
	    // booth queue)
		semaphore_wait(&line_queue_mutex);
		queuePop(line_queue);
		if( queueSize(line_queue) != 0 ) {
			semaphore_post(queuePeek(line_queue));
		}
		semaphore_post(&line_queue_mutex);
	}

	// If you're first in line, leave the line
	else {
		queuePop(line_queue);
		semaphore_post(&line_queue_mutex);
	}

	// Pass your own semaphore into booth queue
	// (Wait in line for a booth)
	semaphore_wait(&booth_queue_mutex);
	queuePush(booth_queue, &my_info->my_sem);

	// If there's a line or there are no open booths, wait in line
	print_status("Independent", my_info->id, my_booth, "Waiting on a voting booth");
	semaphore_wait(&booth_mutex);
	if( queueSize(booth_queue) > 1 || empty_booths <= 0 ) {
		semaphore_post(&booth_mutex);
		semaphore_post(&booth_queue_mutex);
		semaphore_wait(&my_info->my_sem);
		semaphore_wait(&booth_mutex);
	}

	// If you're first in line and there's an empty booth, leave the line
	else {
		queuePop(booth_queue);
		semaphore_post(&booth_queue_mutex);
	}

	// Take a booth
	my_booth = find_booth('I');
	semaphore_post(&booth_mutex);

	// Vote (i.e., sleep for 0–100,000 microseconds)
	print_status("Independent", my_info->id, my_booth, "Voting!");
	usleep(rand() % MAX_VOTE_TIME);

	// Leave your booth
	semaphore_wait(&booth_mutex);
	leave_booth(my_booth);
	semaphore_post(&booth_mutex);
	print_status("Independent", my_info->id, -1, "Leaving the polling station");

	// Let the next person enter the booths
	semaphore_wait(&booth_queue_mutex);
	if( queueSize(booth_queue) != 0 ) {
		semaphore_post(queuePop(booth_queue));
	}
	semaphore_post(&booth_queue_mutex);

	return 0;
}

int is_num(char *num_str) {
	int i = 0;

	while( num_str[i] != '\0' ) {
		if( isdigit(num_str[i]) ) {
			i++;
		}
		else {
			return -1;
		}
	}

	return 0;
}

int parse_args(int argc,
			   char *argv[],
	           int *rep_count,
			   int *dem_count,
			   int *ind_count) {
	booth_count = 10;
	*rep_count = 5;
	*dem_count = 5;
	*ind_count = 5;

	switch( argc ) {
		case 5:
			if( is_num(argv[4]) == 0 ) {
				*ind_count = atoi(argv[4]);
			}
			else {
				fprintf(stderr, "Argument \"%s\" is not a positive integer.\n", argv[4]);
				return -1;
			}
		case 4:
			if( is_num(argv[3]) == 0 ) {
				*dem_count = atoi(argv[3]);
			}
			else {
				fprintf(stderr, "Argument \"%s\" is not a positive integer.\n", argv[3]);
				return -1;
			}
		case 3:
			if( is_num(argv[2]) == 0 ) {
				*rep_count = atoi(argv[2]);
			}
			else {
				fprintf(stderr, "Argument \"%s\" is not a positive integer.\n", argv[2]);
				return -1;
			}
		case 2:
			if( is_num(argv[1]) == 0 ) {
				booth_count = atoi(argv[1]);
			}
			else {
				fprintf(stderr, "Argument \"%s\" is not a positive integer.\n", argv[1]);
				return -1;
			}
		case 1:
			break;
		default:
			fprintf(stderr, "Too many arguments.\n");
			return -1;
	}

	return 0;
}

int main(int argc, char *argv[]) {

	int ret = 0;
	int i = 0;
	int rep_count,
		dem_count,
		ind_count;
	if( parse_args(argc, argv, &rep_count, &dem_count, &ind_count) == 0 ) {

		int total_count = rep_count + dem_count + ind_count;

		printf("Number of Voting Booths\t: %3d\n", booth_count);
		printf("Number of Republicans\t: %3d\n", rep_count);
		printf("Number of Democrats\t: %3d\n", dem_count);
		printf("Number of Independents\t: %3d\n", ind_count);

		// Create threads (all waiting on polling station open)
		if( (ret = init(rep_count, dem_count, ind_count)) != 0 ) {
			fprintf(stderr, "Couldn't initialize.\n");
			exit(ret);
		}

		sleep(2);

		print_divider();

		// Open the polling station (wake up all threads)
		for( i = 0; i < total_count; i++ ) {
			semaphore_post(&barrier);
		}

		for( i = 0; i < rep_count; i++ ) {
			pthread_join(rep_threads[i], NULL);
		}
		for( i = 0; i < dem_count; i++ ) {
			pthread_join(dem_threads[i], NULL);
		}
		for( i = 0; i < ind_count; i++ ) {
			pthread_join(ind_threads[i], NULL);
		}

		print_divider();

		destroy(rep_count, dem_count, ind_count);
	}
	else {
		fprintf(stderr, "Failed to parse arguments. A valid call to this ");
		fprintf(stderr, "program is of the form\n");
		fprintf(stderr, "./finicky-voter <booths> <republicans> <democrats> <independents>\n");
		fprintf(stderr, "where all four arguments are optional positive integers.\n");
		return -1;
	}

	return 0;

}
