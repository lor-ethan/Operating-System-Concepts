/*
 * Josh Hursey, Samantha Foley, Ethan Lor, Tyler Durr
 *
 * CS441/541: Project 3
 *
 */
#ifndef MYSHELL_H
#define MYSHELL_H

#include "support.h"

/* For fork, exec, sleep */
#include <sys/types.h>
#include <unistd.h>
/* For waitpid */
#include <sys/wait.h>
/* For redirection */
#include <fcntl.h>
#include <sys/stat.h>

/******************************
 * Defines
 ******************************/
#define PROMPT ("mysh$ ")


/******************************
 * Structures
 ******************************/
struct job_h {
	char * full_command;
    int is_background;
    char * binary;
    pid_t pid;
	int status;
};
typedef struct job_h job_h;

/******************************
 * Global Variables
 ******************************/
job_t *loc_jobs = NULL;
job_h *job_history = NULL;
int num_jobs = 0;
/*
 * Debugging toggle
 */
int is_debug = FALSE;

/*
 * Interactive or batch mode
 */
int is_batch = FALSE;

/*
 * Batch file names
 */
int num_batch_files = 0;
char **batch_files = NULL;

/*
 * Counts
 */
int total_jobs_display_ctr = 0;
int total_jobs    = 0;
int total_jobs_bg = 0;
int total_history = 0;

/*
 * If we are exiting
 */
int exiting = FALSE;


/******************************
 * Function declarations
 ******************************/
/*
 * Parse command line arguments passed to myshell upon startup.
 *
 * Parameters:
 *  argc : Number of command line arguments
 *  argv : Array of pointers to strings
 *
 * Returns:
 *   0 on success
 *   Negative value on error
 */
int parse_args_main(int argc, char **argv);

/*
 * Main routine for batch mode
 *
 * Parameters:
 *  None
 *
 * Returns:
 *   0 on success
 *   Negative value on error
 */
int batch_mode(void);

/*
 * Main routine for interactive mode
 *
 * Parameters:
 *  None
 *
 * Returns:
 *   0 on success
 *   Negative value on error
 */
int interactive_mode(void);

/*
 * Split possible multiple jobs on a command line, then call parse_and_run()
 *
 * Parameters:
 *  command : command line string (may contain multiple jobs)
 *
 * Returns:
 *   0 on success
 *   Negative value on error
 */
int split_parse_and_run(char * command);

/*
 * Parse and execute a single job given to the prompt.
 *
 * Parameters:
 *   loc_job : job to execute
 *
 * Returns:
 *   0 on success
 *   Negative value on error
 */
int parse_and_run(job_t * loc_job);

/*
 * Launch a job
 *
 * Parameters:
 *   loc_job : job to execute
 *
 * Returns:
 *   0 on success
 *   Negative value on error
 */
int launch_job(job_t * loc_job);

/*
 * Built-in 'exit' command
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   0 on success
 *   Negative value on error
 */
int builtin_exit(void);

/*
 * Built-in 'jobs' command
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   0 on success
 *   Negative value on error
 */
int builtin_jobs(void);

/*
 * Built-in 'history' command
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   0 on success
 *   Negative value on error
 */
int builtin_history(void);

/*
 * Built-in 'wait' command
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   0 on success
 *   Negative value on error
 */
int builtin_wait(void);

/*
 * Built-in 'fg' command
 *
 * Parameters:
 *   None, or job id
 *
 * Returns:
 *   0 on success
 *   Negative value on error
 */
int builtin_fg(int argc, char **argv);

/*
 * Store history of all commands and their arguments
 *
 * Parameters:
 *   loc_job pointer
 *
 * Returns:
 *   0 on success
 */
int add_history(job_t *loc_job);

#endif /* MYSHELL_H */
