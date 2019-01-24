/*
 * CS 441/541: Page Fault Algorithm Simulator (Project 6)
 *
 * Ethan Lor
 * Tyler Durr
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

/******************************
 * Defines
 ******************************/

#define MIN_FRAMES 1
#define MAX_FRAMES 7

/******************************
* Structures
******************************/

struct frame {
   int page;
   int last_used;
   bool second_chance;
   bool dirty;
};
typedef struct frame frame;

struct page_ref {
   int num;
   enum access_type {R, W} type;
};
typedef struct page_ref page_ref;

/******************************
 * Global Variables
 ******************************/

int frame_count = -1;
frame *frames = NULL;
int page_ref_count = -1;
page_ref *page_refs = NULL;

/******************************
 * Function declarations
 ******************************/

int parse_args(int argc, char **argv);
int reset_frames();
int check_frames(page_ref p);
int OPT();
int FIFO();
int LRU();
int checkSC(int start_index);
int checkESC(int start_index, bool dirty_bit, bool set_sc_bit);
int SC(bool enhanced);
int LRU_SC();
int LRU_ESC();
int run_schedulers();
