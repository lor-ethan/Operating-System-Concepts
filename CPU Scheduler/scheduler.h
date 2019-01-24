/*
 * 
 *
 * CS 441/541: CPU Scheduler (Project 1)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>


/******************************
 * Defines
 ******************************/


/******************************
 * Structures
 ******************************/


/******************************
 * Global Variables
 ******************************/
FILE *fp;
char *file;
char buff[255];
int **proc;

/******************************
 * Function declarations
 ******************************/
int is_num(int i, int argc, char*argv[]);
int in_range(int num);
int open_file(char file[]);
int make_array(int size, int col);
int print_stats(int size);
int fcfs(int size);
int shortest_first(int size);
int priority(int size);
int round_robin(int size, int quantum);
