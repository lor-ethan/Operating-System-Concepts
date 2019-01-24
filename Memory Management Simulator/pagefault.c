/*
* CS 441/541: Page Fault Algorithm Simulator (Project 6)
*
* Tyler Durr
* Ethan Lor
*/
#include "pagefault.h"

int parse_args(int argc, char **argv) {
   int i = -1;
   char type;
   char *filename;

   // If no arguments were passed, return an error
   if( argc < 2 ) {
      fprintf(stderr, "Not enough arguments. You must at least pass one file to ");
      fprintf(stderr, "be read from. Additionally, you may also pass in \"-f <frames>\", ");
      fprintf(stderr, "where <frames> is an integer between %i and %i, inclusive.\n", MIN_FRAMES, MAX_FRAMES);
      return -1;
   }

   // Parse the arguments
   for( i = 1; i < argc; i++ ) {

      // "-f <number of frames>"
      if( strcmp(argv[i], "-f") == 0 ) {
         frame_count = atoi(argv[++i]);
         if( frame_count > MAX_FRAMES || frame_count < MIN_FRAMES ) {
            fprintf(stderr, "Invalid number of frames. Use an integer between ");
            fprintf(stderr, "%i and %i, inclusive.\n", MIN_FRAMES, MAX_FRAMES);
            return -2;
         }
      }

      // First filename specified
      else if( page_refs == NULL ){
         filename = argv[i];
         FILE *page_ref_file = fopen(argv[i], "r");

         // Check that file was successfully opened
         if (page_ref_file == NULL) {
            printf("Error: File \"%s\" does not exist or could not be opened.\n", argv[i]);
            return -3;
         }

         // The first value in the file is the size of the file
         fscanf(page_ref_file, "%i\n", &page_ref_count);
         page_refs = malloc(sizeof(page_ref) * page_ref_count);

         // Fill page_refs from file
         for( i = 0; i < page_ref_count; i++ ) {
            fscanf(page_ref_file, "%i %c\n", &(page_refs[i].num), &type);
            if(toupper(type) == 'R') {
               page_refs[i].type = R;
            }
            else {
               page_refs[i].type = W;
            }
         }

         fclose(page_ref_file);
      }

      // Ignore it; it's an extraneous filename
      else {}
   }

   printf("Num. Frames : ");
   if( frame_count == -1 ) {
      printf("All\n");
   }
   else {
      printf("%2d\n", frame_count);
   }
   printf("Ref. File   : %s\n", filename);

   return 0;
}

int reset_frames() {

   int i = 0;

   for( i = 0; i < frame_count; i++ ) {
      frames[i].page = -1;
      frames[i].last_used = -1;
      frames[i].second_chance = false;
      frames[i].dirty = false;
   }

   return 0;
}

// Check frames for hit or miss
int check_frames(page_ref p) {
   int i = 0;
   for(i = 0; i < frame_count; i++) {
      if(p.num == frames[i].page) {
         return i;
      }
   }
   return -1;
}

// Optimal
int OPT() {
   int i = 0;
   int page_faults = 0;
   int repl_index = 0;

   reset_frames();

   for( i = 0; i < page_ref_count; i++ ) {
      int found = check_frames(page_refs[i]);

      // Page fault
      if( found < 0 ) {
         int j;
         if( repl_index < frame_count ) {
            j = repl_index;
            repl_index++;
         }
         else {
            int m = i;
            int n = 0;
            bool used_in_future[frame_count];
            int frames_found = 0;

            for( n = 0; n < frame_count; n++ ) {
               used_in_future[n] = false;
            }

            // Find each page that is used in the future. When only one page has
            // yet to be used or the page references run out, any page whose value
            // in used_in_future is still false will be the optimal page to
            // replace.
            while( ++m < page_ref_count && frames_found < frame_count - 1 ) {
               int k = 0;

               while( k < frame_count && frames[k].page != page_refs[m].num ) {
                  k++;
               }
               if( k < frame_count && !used_in_future[k] ) {
                  used_in_future[k] = true;
                  frames_found++;
               }
            }

            // Now, at least one frame has not been marked as used in the future;
            // find it.
            j = 0;
            while( used_in_future[j] ) {
               j++;
            }
         }

         // The page in frames[j] is the optimal candidate for replacement.
         frames[j].page = page_refs[i].num;
         page_faults++;
      }
   }

   return page_faults;
}

// First-In, First-Out (FIFO)
int FIFO() {
   int i = 0;
   int pointer = 0;
   int page_faults = 0;

   reset_frames();

   for( i = 0; i < page_ref_count; i++ ) {
      int found = check_frames(page_refs[i]);

      // Page fault
      if( found < 0 ) {
         frames[pointer].page = page_refs[i].num;
         pointer = (pointer + 1) % frame_count;
         page_faults++;
      }
   }

   return page_faults;
}

// Least-Recently Used (LRU)
int LRU() {
   int repl_index = 0;
   int page_faults = 0;
   int i = 0;
   int j = 0;

   reset_frames();

   for(i = 0; i < page_ref_count; i++) {
      // Check if hit or miss
      int frame_index = check_frames(page_refs[i]);
      if(frame_index < 0) { // miss
         if(frames[repl_index].page == -1) {
            // Pointer is open
            frame_index = repl_index;
         } else {
            // Find least recently used
            int LRU_index = 0;
            int LRU_count = frames[0].last_used;
            for(j = 1; j < frame_count; j++) {
               if(frames[j].last_used < LRU_count) {
                  LRU_index = j;
                  LRU_count = frames[j].last_used;
               }
            }
            frame_index = LRU_index;
         }
         frames[frame_index].page = page_refs[i].num;
         repl_index = (repl_index+1) % frame_count;
         page_faults++;
      }
      frames[frame_index].last_used = i;
   }

   return page_faults;
}

// Check Second Chance
int checkSC(int start_index) {
   int i = start_index;
   do {
      if(!frames[i].second_chance) {
         return i;
      } else {
         frames[i].second_chance = false;
      }
      i = (i+1) % frame_count;
   } while(true);
   return -1;
}

int checkESC(int start_index, bool dirty_bit, bool set_sc_bit) {
   int i = start_index;
   do {
      if(!frames[i].second_chance && frames[i].dirty == dirty_bit) {
         return i;
      }
      else {
         if(set_sc_bit) {
            frames[i].second_chance = false;
         }
      }
      i = (i+1) % frame_count;
   } while(i != start_index);
   return -1;
}

int SC(bool enhanced) {
   // Need a new/clean "page table"?
   int repl_index = 0;
   int page_faults = 0;
   int i = 0;

   reset_frames();

   for(i = 0; i < page_ref_count; i++) {
      // Check if hit or miss
      int frame_index = check_frames(page_refs[i]);

      if(frame_index >= 0) {
         // Hit - Update R/W bit and dirty bit
         frames[frame_index].second_chance = true;
         if(enhanced && page_refs[i].type == W) {
            frames[frame_index].dirty = true;
         }
      } else {
         if(frames[repl_index].page == -1) {
            frame_index = repl_index;
         }
         else {
            if(enhanced) {
               // Check for (0 0)
               frame_index = checkESC(repl_index, false, false);
               if(frame_index < 0) {
                  // Check for (0 1) - Also decrement used
                  frame_index = checkESC(repl_index, true, true);
                  if(frame_index < 0) {
                     // Check for (0 0) - Repeat of check1
                     frame_index = checkESC(repl_index, false, false);
                     if(frame_index < 0) {
                        // Check for (0 1)
                        frame_index = checkESC(repl_index, true, false);
                     }
                  }
               }
            } else {
               // Check for 0, setting to 0 until one is found
               frame_index = checkSC(repl_index);
            }
         }

         // Put page into "page table"
         frames[frame_index].page = page_refs[i].num;
         frames[frame_index].second_chance = true;
         if(enhanced) {
            frames[frame_index].dirty = (page_refs[i].type == W);
         }
         repl_index = (frame_index+1) % frame_count;

         page_faults++;
      }
   }

   return page_faults;
}

// LRU Second-Chance (LRU SC)
int LRU_SC() {
   return SC(false);
}

// LRU Enhanced Second-Chance LRU
int LRU_ESC() {
   return SC(true);
}

int run_schedulers() {
   printf("#    %7d %7d %7d %7d %7d %7d\n", frame_count, OPT(), FIFO(), LRU(), LRU_SC(), LRU_ESC());

   return 0;
}

int main(int argc, char **argv) {
   int ret;
   int i = 0;
   frames = malloc(sizeof(frame) * MAX_FRAMES);

   // Read in arguments and files
   ret = parse_args(argc, argv);
   if( ret ) {
      fprintf(stderr, "Could not parse argument; error %i.\n", ret);
      return ret;
   }

   printf("-------------------------------\n");
   printf("Reference String:");
   for( i = 0; i < page_ref_count - 1; i++ ) {
      if( i % 10 == 0 ) {
         if( i != 0 ) {
            printf(", ");
         }
         printf("\n ");
      }
      else {
         printf(", ");
      }
      printf("%2d:%c", page_refs[i].num, (page_refs[i].type == R ? 'R' : 'W'));
   }
   printf(", %2d:%c\n", page_refs[i].num, (page_refs[i].type == R ? 'R' : 'W'));
   printf("-------------------------------\n");
   printf("####################################################\n");
   printf("#     Frames    Opt.    FIFO     LRU      SC     ESC\n");
   if( frame_count == -1 ) {
      for( i = MIN_FRAMES; i <= MAX_FRAMES; i++ ) {
         frame_count = i;
         run_schedulers();
      }
   }
   else {
      run_schedulers();
   }
   printf("####################################################\n"); fflush(NULL);

   free(frames);

   return 0;
}
