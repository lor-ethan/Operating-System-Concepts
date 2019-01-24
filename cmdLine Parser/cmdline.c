/*
 * Ethan Lor
 * February 5, 2018
 *
 * Program that parses a command line arguments for the correct options and arguments.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

int iterations(int i,int argc, char *argv[]);
int stage(int i, int argc, char *argv[]);
int print(int iter, int bug, char *name);

int main(int argc, char **argv) {
    char param1[3] = "-n";
    char param2[3] = "-d";
    char param3[7] = "-stage";
    int iter = 0;
    int bug = 1;
    char *name = NULL;

    if(argc == 1) {
        printf("Error: -n option required to be specified.\n");
        return -1;
    }

    int i = 1;
    while (i < argc) {
        if(strcmp(param1, argv[i]) == 0) {
            if(iterations(i, argc, argv) == 0) {
                iter = atoi(argv[i+1]);
                i = i + 2;
            } else {
                return -1;
            }
        }else if((strcmp(param2, argv[i]) == 0) ) {
            bug = 0;
            i++;
        }else if((strcmp(param3, argv[i]) == 0) ) {
            if(stage(i, argc, argv) == 0) {
                name = strdup(argv[i+1]);
                i = i + 2;
            } else {
                return -1;
            }
        } else {
            printf("Error: Unknown option: %s\n", argv[i]);
            return -1;
        }
    }

    if(iter == 0) {
        printf("Error: -n option required to be specified.\n");
        return -1;
    }

    print(iter, bug, name);
    return 0;
}


int iterations(int i, int argc, char *argv[]) {
    if(i >= argc-1){
        printf("Error: Not enough arguments to the -n option.\n");
        return -1;
    }
    char *str = argv[i+1];
    if(str[0] == '-') {
       printf("Error: -n option requires a positive integer greater than 0\n");
       return -1;
    }
    int j;
    for(j = 0; j < strlen(str); j++) {
        if(!isdigit(str[j])) {
            printf("Error: -n option requires a positive integer greater than 0\n");
            return -1;
        }
    }
    if(atoi(argv[i+1]) <= 0) {
        printf("Error: -n option requires a positive integer greater than 0\n");
        return -1;
    }
    return 0;
}

int stage(int i, int argc, char *argv[]) {
    if(i >= argc-1) {
        printf("Error: Not enough arguments to the -stage option.\n");
        return -1;
    }
    char *name = strdup(argv[i+1]);
    int j;
    for(j = 0; j < strlen(name); j++) {
        if(name[j] < 'A' || name[j] > 'Z') {
            printf("Error: -stage option reqiures a string NAME\n");
            return -1;
        }
    }
    free(name);
    name = NULL;
    return 0;
}

int print(int iter, int bug, char *name) {
    printf("#-----------------------\n");
    printf("# Iterations : %d\n", iter);
    if(bug == 0) {
        printf("# Debugging  : Enabled\n");
    } else {
        printf("# Debugging  : Disabled (Default)\n");
    }
    if(name == NULL) {
        printf("# Stage(s)   : All (Default)\n");
    } else {
        printf("# Stage(s)   : %s\n", name);
        free(name);
        name = NULL;
    }
    printf("#-----------------------\n");
    return 0;
}
