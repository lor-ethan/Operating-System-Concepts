/*
 * Josh Hursey, Samantha Foley, Ethan Lor, Tyler Durr
 *
 * CS441/541: Project 3
 *
 */
#include "mysh.h"

int main(int argc, char * argv[]) {
	int ret = 0;
    job_history = (job_h*)malloc(sizeof(job_h) * 1);

    if(NULL == job_history) {
      printf("Memory allocation failed");
      exit(1);
    }

    /*
     * Parse Command line arguments to check if this is an interactive or batch
     * mode run.
     */
    if( 0 != (ret = parse_args_main(argc, argv)) ) {
        fprintf(stderr, "Error: Invalid command line!\n");
        return -1;
    }

    /*
     * If in batch mode then process all batch files
     */
    if( TRUE == is_batch) {
        if( TRUE == is_debug ) {
            printf("Batch Mode!\n");
        }

        if( 0 != (ret = batch_mode()) ) {
            fprintf(stderr, "Error: Batch mode returned a failure!\n");
        }
    }
    /*
     * Otherwise proceed in interactive mode
     */
    else if( FALSE == is_batch ) {
        if( TRUE == is_debug ) {
            printf("Interactive Mode!\n");
        }

        if( 0 != (ret = interactive_mode()) ) {
            fprintf(stderr, "Error: Interactive mode returned a failure!\n");
        }
    }
    /*
     * This should never happen, but otherwise unknown mode
     */
    else {
        fprintf(stderr, "Error: Unknown execution mode!\n");
        return -1;
    }


    /*
     * Display counts
     */
    printf("-------------------------------\n");
    printf("Total number of jobs               = %d\n", total_jobs);
    printf("Total number of jobs in history    = %d\n", total_history);
    printf("Total number of jobs in background = %d\n", total_jobs_bg);

    /*
     * Cleanup
     */
    if( NULL != batch_files) {
        free(batch_files);
        batch_files = NULL;
        num_batch_files = 0;
    }

    return 0;
}

int parse_args_main(int argc, char **argv)
{
    int i;

    /*
     * If no command line arguments were passed then this is an interactive
     * mode run.
     */
    if( 1 >= argc ) {
        is_batch = FALSE;
        return 0;
    }

    /*
     * If command line arguments were supplied then this is batch mode.
     */
    is_batch = TRUE;
    num_batch_files = argc - 1;
    batch_files = (char **) malloc(sizeof(char *) * num_batch_files);
    if( NULL == batch_files ) {
        fprintf(stderr, "Error: Failed to allocate memory! Critical failure on %d!", __LINE__);
        exit(-1);
    }

    for( i = 1; i < argc; ++i ) {
        batch_files[i-1] = strdup(argv[i]);
    }

    return 0;
}

int batch_mode(void)
{
    int i;
    int ret;
    char * command = NULL;
    char * cmd_rtn = NULL;
    FILE *batch_fd = NULL;

    command = (char *) malloc(sizeof(char) * (MAX_COMMAND_LINE+1));
    if( NULL == command ) {
        fprintf(stderr, "Error: Failed to allocate memory! Critical failure on %d!", __LINE__);
        exit(-1);
    }

    for(i = 0; i < num_batch_files; ++i) {
        if( TRUE == is_debug ) {
            printf("-------------------------------\n");
            printf("Processing Batch file %2d of %2d = [%s]\n", i, num_batch_files, batch_files[i]);
            printf("-------------------------------\n");
        }

        /*
         * Open the batch file
         * If there was an error then print a message and move on to the next file.
         */
        if( NULL == (batch_fd = fopen(batch_files[i], "r")) ) {
            fprintf(stderr, "Error: Unable to open the Batch File [%s]\n", batch_files[i]);
            continue;
        }

        /*
         * Read one line at a time.
         */
        while( FALSE == exiting && 0 == feof(batch_fd) ) {

            /* Read one line */
            command[0] = '\0';
            if( NULL == (cmd_rtn = fgets(command, MAX_COMMAND_LINE, batch_fd)) ) {
                break;
            }

            /* Strip off the newline */
            if( '\n' == command[strlen(command)-1] ) {
                command[strlen(command)-1] = '\0';
            }

            /*
             * Parse and execute the command
             */
            if( 0 != (ret = split_parse_and_run(command)) ) {
                fprintf(stderr, "Error: Unable to run the command \"%s\"\n", command);
            }
        }

        /*
         * Close the batch file
         */
        fclose(batch_fd);
    }

    /*
     * Cleanup
     */
    if( NULL != command ) {
        free(command);
        command = NULL;
    }

    return 0;
}

int interactive_mode(void)
{
    int ret;
    char * command = NULL;
    char * cmd_rtn = NULL;

    /*
     * Display the prompt and wait for input
     */
    command = (char *) malloc(sizeof(char) * (MAX_COMMAND_LINE+1));
    if( NULL == command ) {
        fprintf(stderr, "Error: Failed to allocate memory! Critical failure on %d!", __LINE__);
        exit(-1);
    }

    do {
        /*
         * Print the prompt
         */
        printf("%s", PROMPT);
        fflush(NULL);

        /*
         * Read stdin, break out of loop if Ctrl-D
         */
        command[0] = '\0';
        if( NULL == (cmd_rtn = fgets(command, MAX_COMMAND_LINE, stdin)) ) {
            printf("\n");
            fflush(NULL);
            break;
        }

        /* Strip off the newline */
        if( '\n' == command[strlen(command)-1] ) {
            command[strlen(command)-1] = '\0';
        }

        /*
         * Parse and execute the command
         */
        if( 0 != (ret = split_parse_and_run(command)) ) {
            fprintf(stderr, "Error: Unable to run the command \"%s\"\n", command);
            /* This is not critical, just try the next command */
        }

    } while( NULL != cmd_rtn && FALSE == exiting);

    /*
     * Cleanup
     */
    if( NULL != command ) {
        free(command);
        command = NULL;
    }

    return 0;
}

int split_parse_and_run(char * command)
{
    int ret, i;
    int    num_jobs = 0;
    job_t *loc_jobs = NULL;
    char * dup_command = NULL;
    int bg_idx;
    int valid = FALSE;

    /*
     * Sanity check
     */
    if( NULL == command ) {
        return 0;
    }

    /*
     * Check for multiple sequential or background operations on the same
     * command line.
     */
    /* Make a duplicate of command so we can sort out a mix of ';' and '&' later */
    dup_command = strdup(command);

    /******************************
     * Split the command into individual jobs
     ******************************/
    /* Just get some space for the function to hold onto */
    loc_jobs = (job_t*)malloc(sizeof(job_t) * 1);
    if( NULL == loc_jobs ) {
        fprintf(stderr, "Error: Failed to allocate memory! Critical failure on %d!", __LINE__);
        exit(-1);
    }

    split_input_into_jobs(command, &num_jobs, &loc_jobs);

    /*
     * For each job, check for background or foreground
     * Walk the command string looking for ';' and '&' to identify each job as either
     * sequential or background
     */
    bg_idx = 0;
    valid = FALSE;
    for(i = 0; i < strlen(dup_command); ++i ) {
        /* Sequential separator */
        if( dup_command[i] == ';' ) {
            if( TRUE == valid ) {
                loc_jobs[bg_idx].is_background = FALSE;
                ++bg_idx;
                valid = FALSE;
            }
            else {
                fprintf(stderr, "Error: syntax error near unexpected token ';'\n");
            }
        }
        /* Background separator */
        else if( dup_command[i] == '&' ) {
            if( TRUE == valid ) {
                loc_jobs[bg_idx].is_background = TRUE;
                ++bg_idx;
                valid = FALSE;
            }
            else {
                fprintf(stderr, "Error: syntax error near unexpected token '&'\n");
            }
        }
        /*
         * Look for valid characters. So we can print an error if the user
         * types: date ; ; date
         */
        else if( dup_command[i] != ' ' ) {
            valid = TRUE;
        }
    }

    /*
     * For each job, parse and execute it
     */
    for( i = 0; i < num_jobs; ++i ) {
        if( 0 != (ret = parse_and_run( &loc_jobs[i] )) ) {
            fprintf(stderr, "Error: The following job failed! [%s]\n", loc_jobs[i].full_command);
        }
    }

    /*
     * Cleanup
     */
    if( NULL != loc_jobs ) {
        /* Cleanup struct fields */
        for( i = 0; i < num_jobs; ++i ) {
            if( NULL != loc_jobs[i].full_command ) {
                free( loc_jobs[i].full_command );
                loc_jobs[i].full_command = NULL;
            }

            if( NULL != loc_jobs[i].argv ) {
				int j;
                for( j = 0; j < loc_jobs[i].argc; ++j ) {
                    if( NULL != loc_jobs[i].argv[j] ) {
                        free( loc_jobs[i].argv[j] );
                        loc_jobs[i].argv[j] = NULL;
                    }
                }
                free( loc_jobs[i].argv );
                loc_jobs[i].argv = NULL;
            }

            loc_jobs[i].argc = 0;

            if( NULL != loc_jobs[i].binary ) {
                free( loc_jobs[i].binary );
                loc_jobs[i].binary = NULL;
            }
        }
        /* Free the array */
        free(loc_jobs);
        loc_jobs = NULL;
    }

    if( NULL != dup_command ) {
        free(dup_command);
        dup_command = NULL;
    }

    return 0;
}

int parse_and_run(job_t * loc_job)
{
    int ret = 1;

    /*
     * Sanity check
     */
    if( NULL == loc_job ||
        NULL == loc_job->full_command ) {
        return 0;
    }

    /*
     * No command specified
     */
    if(0 >= strlen( loc_job->full_command ) ) {
        return 0;
    }

    if( TRUE == is_debug ) {
        printf("\t\"%s\"\n", loc_job->full_command );
    }

    ++total_history;

    /******************************
     * Parse the string into the binary, and argv
     ******************************/
    if( split_job_into_args(loc_job) ) {
		// There was an issue reading in redirections;
		// an error has already been printed
		return -1;
	}

    /* Check if the command was just spaces */
    if( 0 >= loc_job->argc ) {
        return 0;
    }

    /* Grab the binary from the list of arguments */
    if( 0 < loc_job->argc ) {
        loc_job->binary = strdup(loc_job->argv[0]);
    }

    /******************************
     * Check for built-in commands:
     * - jobs
     * - exit
     * - history
     * - wait
     * - fg
     ******************************/
    if( 0 == strncmp("exit", loc_job->binary, strlen(loc_job->binary)) ) {
        if( 0 != (ret = builtin_exit() ) ) {
            fprintf(stderr, "Error: exit command failed!\n");
        }
		loc_job->status = SHOWN;
    }
    else if( 0 == strncmp("jobs", loc_job->binary, strlen(loc_job->binary)) ) {
        if( 0 != (ret = builtin_jobs() ) ) {
            fprintf(stderr, "Error: jobs command failed!\n");
        }
		loc_job->status = SHOWN;
    }
    else if( 0 == strncmp("history", loc_job->binary, strlen(loc_job->binary)) ) {
        if( 0 != (ret = builtin_history() ) ) {
            fprintf(stderr, "Error: history command failed!\n");
        }
		loc_job->status = SHOWN;
    }
    else if( 0 == strncmp("wait", loc_job->binary, strlen(loc_job->binary)) ) {
        if( 0 != (ret = builtin_wait() ) ) {
            fprintf(stderr, "Error: wait command failed!\n");
        }
		loc_job->status = SHOWN;
    }
    else if( 0 == strncmp("fg", loc_job->binary, strlen(loc_job->binary)) ) {
        if( 0 != (ret = builtin_fg(loc_job->argc, loc_job->argv) ) ) {
            fprintf(stderr, "Error: fg command failed!\n");
        }
		loc_job->status = SHOWN;
    }

    /*
     * Launch the job
     */
    else {
        if( 0 != (ret = launch_job(loc_job)) ) {
            fprintf(stderr, "Error: Unable to launch the job! \"%s\"\n", loc_job->binary);
        }
    }

    add_history(loc_job);

    return 0;
}

int launch_job(job_t * loc_job)
{
    int i;
	int child_status;
    pid_t newPID;
	int in = 1, out;

    if( is_debug ) {
		printf("Job %2d%c: \"%s\" ",
	           total_jobs_display_ctr + 1,
	           (TRUE == loc_job->is_background ? '*' : ' '),
	           loc_job->binary);
	    fflush(NULL);

	    for( i = 1; i < loc_job->argc; ++i ) {
	        printf(" [%s]", loc_job->argv[i]);
	        fflush(NULL);
	    }
	    printf("\n");
	    fflush(NULL);
	}

    /*
     * Launch the job in either the foreground or background
     */
     if ( (newPID = fork()) > 0 ) { // parent
		 loc_job->pid = newPID;

		 // Wait for foregrounded processes
		 if ( loc_job->is_background == FALSE ) {
			 waitpid(loc_job->pid, &child_status, 0);
			 loc_job->status = child_status ? SHOWN : DONE;
		 }
     }
	 else { // child
		 // Check for redirection
		 if( loc_job->in_file ) {
			 in = open(loc_job->in_file, O_RDONLY);
		 }
		 if( in >= 0 ) {
			 if( loc_job->in_file ) {
				 dup2(in, STDIN_FILENO);
				 close(in);
			 }
			 if( loc_job->out_file ) {
				 out = open(loc_job->out_file, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
				 dup2(out, STDOUT_FILENO);
				 close(out);
			 }
			 if( execvp(loc_job->binary, loc_job->argv) ) { // Never runs unless execvp throws error
				 fprintf(stderr, "Error: Command \"%s\" was not valid.\n", loc_job->full_command);
				 loc_job->status = SHOWN;
				 exit(-1);
			 }
		 }
		 else {
			 fprintf(stderr, "Error: File \"%s\" was not found.\n", loc_job->in_file);
			 loc_job->status = SHOWN;
			 exit(-1);
		 }
     }

    /*
     * Some accounting
     */
    ++total_jobs;
    ++total_jobs_display_ctr;
    if( TRUE == loc_job->is_background ) {
        ++total_jobs_bg;
    }

    return 0;
}

/*
 * Add the job to history
 */
int add_history(job_t *loc_job) {
  job_history = (job_h*)realloc(job_history, (sizeof(job_h) * ((num_jobs) + 1)));

  if(NULL == job_history) {
    printf("Memory allocation failed");
    exit(1);
  }

  job_history[num_jobs].full_command = strdup(loc_job->full_command);
  job_history[num_jobs].is_background = loc_job->is_background;
  job_history[num_jobs].binary = strdup(loc_job->binary);
  job_history[num_jobs].pid= loc_job->pid;
  job_history[num_jobs].status= loc_job->status;
  num_jobs++;

  return 0;
}

/*
 * exit command
 */
int builtin_exit(void)
{
	int i, jobs_running = 0;
    exiting = TRUE;

	if( is_debug ) {
	    printf("Job %2dx: \"exit\"\n", total_jobs_display_ctr + 1);
	    ++total_jobs_display_ctr;
	    fflush(NULL);
	}

	// Update the status of all jobs
	for( i = 0; i < total_history; i++ ) {
		if( job_history[i].status == RUNNING ) {
			if( waitpid(job_history[i].pid, NULL, WNOHANG) != 0 ) {
				job_history[i].status = DONE;
			}
			else {
				jobs_running++;
			}
		}
	}

	if( jobs_running ) {
		printf("Waiting on %d job%s to finish running in the background!\n",
		       jobs_running,
		       (jobs_running == 1 ? "" : "s"));
		fflush(NULL);
	}

	// Wait for all processes to complete before terminating
	for( i = 0; i < num_jobs; i++ ) {
		if ( job_history[i].status == RUNNING ) {
			waitpid(job_history[i].pid, NULL, 0);
			job_history[i].status = DONE;
		}
	}

	if ( job_history ) {
        /* Cleanup struct fields */
        for( i = 0; i < num_jobs; ++i ) {
            if( job_history[i].full_command ) {
                free( job_history[i].full_command );
                job_history[i].full_command = NULL;
            }
            if( job_history[i].binary ) {
                free( job_history[i].binary );
                job_history[i].binary = NULL;
            }
        }
		free( job_history );
		job_history = NULL;
	}

    return 0;
}

/*
 * jobs command
 */
int builtin_jobs(void)
{
	int i, child_status;

	if( is_debug ) {
	    printf("Job %2dx: \"jobs\"\n", total_jobs_display_ctr + 1);
	    ++total_jobs_display_ctr;
	    fflush(NULL);
	}

	// Check the status of running tasks
	for( i = 0; i < total_history; i++ ) {
		if( job_history[i].status == RUNNING ) {
			if( waitpid(job_history[i].pid, &child_status, WNOHANG) != 0 ) {
				job_history[i].status = (child_status ? SHOWN : DONE);
			}
		}
	}

	// Display
	for( i = 0; i < num_jobs; i++ ) {
		if( job_history[i].status != SHOWN &&
		    strncmp("jobs", job_history[i].binary, strlen(job_history[i].binary)) ) {
			printf("[%i] \t%-7s \t%s",
			       i+1,
				   (job_history[i].status == RUNNING ? "Running" : "Done"),
				   job_history[i].full_command);
			printf("\n");
		}
	}

	// Update tasks which were DONE to SHOWN
	for( i = 0; i < num_jobs; i++ ) {
		if( job_history[i].status == DONE ) {
			job_history[i].status = SHOWN;
		}
	}

    return 0;
}

/*
 * history command
 */
int builtin_history(void)
{
	int i;

	if( is_debug ) {
	    printf("Job %2dx: \"history\"\n", total_jobs_display_ctr + 1);
	    ++total_jobs_display_ctr;
	    fflush(NULL);
	}

	// Display
	for( i = 0; i < num_jobs; i++ ) {
		printf("%4i\t%s", i+1, job_history[i].full_command);
		if( job_history[i].is_background ) {
			printf("&");
		}
		printf("\n");
	}
	printf("%4i\t%s\n", i+1, "history");
	fflush(NULL);

    return 0;
}

/*
 * wait command
 */
int builtin_wait(void)
{
	int i;

	if( is_debug ) {
	    printf("Job %2dx: \"wait\"\n", total_jobs_display_ctr + 1);
	    ++total_jobs_display_ctr;
	    fflush(NULL);
	}

	for( i = 0; i < num_jobs; i++ ) {
		if ( job_history[i].status == RUNNING ) {
			waitpid(job_history[i].pid, NULL, 0);
			job_history[i].status = DONE;
		}
	}

    return 0;
}

/*
 * fg command
 */
int builtin_fg(int argc, char **argv)
{
	int job, index;

	if( is_debug ) {
	    printf("Job %2dx: \"fg\"\n", total_jobs_display_ctr + 1);
	    ++total_jobs_display_ctr;
	    fflush(NULL);
	}

	if( argc > 1 ) {
		job = atoi(argv[1]);
		index = job - 1;
		if( index < 0 || index > num_jobs ) {
			return -1;
		}
		if( job_history[index].status == RUNNING ) {
			if( waitpid(job_history[index].pid, NULL, WNOHANG) != 0 ) {
				printf("[%d]\tHas completed\n", job);
				job_history[index].status = SHOWN;
			}
			else {
				waitpid(job_history[index].pid, NULL, 0);
				job_history[index].status = DONE;
			}
		}
		else {
			printf("[%d]\tHas completed\n", job);
			job_history[index].status = SHOWN;
		}
	}
	else {
		int i = num_jobs - 1;
		int index = -1;
		int found = 0;
		while( found == 0 && i >= 0 ) {
			if( job_history[i].is_background == TRUE && job_history[i].status == RUNNING ) {
				if( !waitpid(job_history[i].pid, NULL, WNOHANG) ) {
					index = i;
					found = 1;
				}
				else {
					job_history[i].status = DONE;
				}
			}
			i--;
		}
		if( index == -1 ) {
			printf("No background jobs\n");
		}
		else {
			waitpid(job_history[index].pid, NULL, 0);
			job_history[index].status = DONE;
		}
	}

    return 0;
}
