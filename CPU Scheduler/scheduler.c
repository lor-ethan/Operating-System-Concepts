/*
 * Ethan Lor
 * February 13, 2018 
 *
 * CS 441/541: CPU Scheduler (Project 2)
 */
#include "scheduler.h"

int main(int argc, char **argv) {
    char s_param[3] = "-s";
    char q_param[3] = "-q";
    file = NULL;
    int i = 1;
    int required = -1;
    int algorithm;
    int quantum = 0;
    int size;
    
    if(argc == 1) {
        printf("Error: -s option required to be specified.\n");
        return -1;
    }

    while (i < argc) {
        if(strcmp(argv[i], s_param) == 0) {
            //printf("have -s\n");
            if(is_num(i, argc, argv) == 0) {
                algorithm = atoi(argv[i+1]);
                if(algorithm > 0 && algorithm < 5) {
                    required = 0;
                    i = i + 2;
                } else {
                    printf("Error: -s option requires an integer value between 1 and 4\n");
                    return -1;
                }
            } else {
                printf("Error: -s option requires an integer value between 1 and 4\n");
                return -1;
            }
        } else if(strcmp(argv[i], q_param) == 0) {
            //printf("have -q\n");
            if(is_num(i, argc, argv) == 0) {
                quantum = atoi(argv[i+1]);
                i = i + 2;
            } else {
                printf("Error: -q option requires a positive interger value.\n");
                return -1;
            }
        } else {
            if(file == NULL) {
                file = strdup(argv[i]);
                //printf("'%s' is the string.\n", argv[i]);
                i++;
            } else {
                printf("Error: '%s' is not a reconized option.\n", argv[i]);
                return -1;
            } 
        }
    }
    
    if(required != 0) {
        printf("Error: -s option required to be specified.\n");
        return -1;
    } else {
        if(algorithm == 4 && quantum == 0) {
            printf("Error: -q option and argument must be specified with round robin option -s 4\n");
            return -1;
        }
        if(file == NULL) {
            printf("Error: a text file is required to be specified.\n");
            return -1;
        }
    }

    if(open_file(file) != 0) {
        return -1;
    } else {
        fscanf(fp, "%s", buff);
        size = atoi(buff);
    }

    if(algorithm == 1) {
        fcfs(size);
    } else if(algorithm == 2) {
        shortest_first(size);
    } else if(algorithm == 3) {
        priority(size);
    } else if(algorithm == 4) {
        round_robin(size, quantum);
    }

    //printf("%d\n", proc[0][3]);
    //printf("%d\n", proc[0][4]);
    //printf("%d\n", proc[1][3]);
    //printf("%d\n", proc[1][4]);
    //printf("%d\n", proc[2][3]);
    //printf("%d\n", proc[2][4]);
    //printf("%d\n", proc[3][3]);
    //printf("%d\n", proc[3][4]);

    print_stats(size);
    
    return 0;
}


int is_num(int i, int argc, char *argv[]) {
    if(i < argc-1) {
        char *num = strdup(argv[i+1]);
        int index = 0;
      
        while(num[index] != '\0') {
            if(isdigit(num[index])) {
                //printf("'%c' is a digit\n", num[index]);
                index++;
            } else {
                //printf("'%c' is not a digit\n", num[index]);
                free(num);
                num = NULL;
                return -1;
            }
        }
        free(num);
        num = NULL;
        return 0;
    } else {
        return -1;
    }
}


int open_file(char file[]) {
    fp = fopen(file, "r");
  
    if (fp == NULL) {
        printf("Error: '%s' file does not exist or could not be opened.\n", file);
        return -1;
    }
    return 0;
}


int make_array(int size, int col) {
    proc = (int**)malloc(sizeof(int)*size);
    int i = 0;

    while(i < size) {
        proc[i] = (int*)malloc(sizeof(int)*col);
        int j = 0;
        while(j < col) {
            proc[i][j] = 0;
            j++;
        }
        i++;
    }
    return 0;
}


int run_print(int a, int size, int quant) {
    int i = 0;
    char algo[9];
  
    if(a == 1) {
        strcpy(algo, "FCFS");
    } else if(a == 2) {
        strcpy(algo, "SJF");
    } else if(a == 3) {
        strcpy(algo, "Priority");
    } else {
        strcpy(algo, "RR");
    }

    printf("Scheduler   :  %d %s\n",a,algo);
    printf("Quantum     :  %d\n",quant);
    printf("Sch. File   : %s\n",file);
    printf("-------------------------------\n");
    printf("Arrival Order:");
    
    while(i < size) {
        if(i == size - 1) {
            printf("%3d\n", proc[i][0]);
        } else {
            printf("%3d,", proc[i][0]);
        }
        i++;
    }

    printf("Process Information:\n");

    i = 0;
    while(i < size) {
        printf("%2d%9d%8d\n", proc[i][0],proc[i][1],proc[i][2]);
        i++;
    }

    printf("-------------------------------\n");
    printf("Running...\n");

    return 0;
}


int print_stats(int size) {
    int wait = 0;
    int turn = 0;
    int i = 0;
    double avg_wait;
    double avg_turn;
    
    while(i < size) {
        wait += proc[i][3];
        turn += proc[i][4];
        i++;
    }
   
    avg_wait = (double)wait/size;
    avg_turn = (double)turn/size;

    printf("##################################################\n");
    printf("#%3s%7s%8s%8s%8s\n","#","CPU","Pri","W","T");
   
    i = 0;
    while(i < size) {
        printf("#%3d%7d%8d%8d%8d\n",proc[i][0],proc[i][1],proc[i][2],proc[i][3],proc[i][4]); 
        i++;
    }
    
    printf("##################################################\n");
    printf("# Avg. Waiting Time   :%7.02f\n",avg_wait);
    printf("# Avg. Turnaround Time:%7.02f\n",avg_turn);
    printf("##################################################\n");
    
    return 0;
}


int fcfs(int size) {
    int i = 0;
    int counter = 0;
    make_array(size, 5);
    
    while(i < size) {
        fscanf(fp, "%s", buff);
        proc[i][0] = atoi(buff);
        fscanf(fp, "%s", buff);
        proc[i][1] = atoi(buff);
        fscanf(fp, "%s", buff);
        proc[i][2] = atoi(buff);
        proc[i][3] = 0;
        proc[i][4] = 0;
        i++;
        //   printf("HERE!");
    }
    
    run_print(1, size, 0);

    i = 0;
    while(i < size) {
        proc[i][3] = counter;
        counter += proc[i][1];
        proc[i][4] = counter;
        i++;
    }

    return 0;
}


int shortest_first(int size) {
    int i = 0;
    int complete = 0;
    int index = 0;
    int shortest;
    int counter = 0;
    make_array(size, 6);

    while(i < size) {
        fscanf(fp, "%s", buff);
        proc[i][0] = atoi(buff);
        fscanf(fp, "%s", buff);
        proc[i][1] = atoi(buff);
        fscanf(fp, "%s", buff);
        proc[i][2] = atoi(buff);
        proc[i][3] = 0;
        proc[i][4] = 0;
        proc[i][5] = -1;
        i++;
        //   printf("HERE!");
    }

    run_print(2, size, 0);
    
    while(complete < size) {
        shortest = -1;
        i = 0;
        while(i < size) {
            if(proc[i][5] != 0) {
                if(shortest == -1 || proc[i][1] < shortest) {
                    index = i;
                    shortest = proc[i][1];
                }
            }
            i++;
        }
        proc[index][3] = counter;
        counter += proc[index][1];
        proc[index][4] = counter;
        proc[index][5] = 0;
        complete++;
    }
   
    return 0;
}


int priority(int size) {
    int i = 0;
    int complete = 0;
    int index = 0;
    int least;
    int counter = 0;
    make_array(size, 6);
    
    while(i < size) {
        fscanf(fp, "%s", buff);
        proc[i][0] = atoi(buff);
        fscanf(fp, "%s", buff);
        proc[i][1] = atoi(buff);
        fscanf(fp, "%s", buff);
        proc[i][2] = atoi(buff);
        proc[i][3] = 0;
        proc[i][4] = 0;
        proc[i][5] = -1;
        i++;
        //   printf("HERE!");                                                                          
    }

    run_print(3, size, 0);

    while(complete < size) {
        least = -1;
        i = 0;
        while(i < size) {
            if(proc[i][5] != 0) {
                if(least == -1 || proc[i][2] < least) {
                    index = i;
                    least = proc[i][2];
                }
            }
            i++;
        }
        proc[index][3] = counter;
        counter += proc[index][1];
        proc[index][4] = counter;
        proc[index][5] = 0;
        complete++;
    }

    return 0;
}


 int round_robin(int size, int quantum) {
    int i = 0;
    int counter = 0;
    int complete = 0;
    make_array(size, 7);

    while(i < size) {
        fscanf(fp, "%s", buff);
        proc[i][0] = atoi(buff);
        fscanf(fp, "%s", buff);
        proc[i][1] = atoi(buff);
        fscanf(fp, "%s", buff);
        proc[i][2] = atoi(buff);
        proc[i][3] = 0;
        proc[i][4] = 0;
        proc[i][5] = proc[i][1];
        proc[i][6] = -1;
        i++;
        //printf("HERE!");
    }

    run_print(4, size, quantum);

    while(complete < size) {
        //printf("while\n");
        i = 0;
        while(i < size && complete < size) {
            if(proc[i][6] != 0) {
                if(proc[i][5] > quantum) {
                    //printf("greater than quantum!\n");
                    proc[i][5] = proc[i][5] - quantum;
                    proc[i][3] = counter - proc[i][4] + proc[i][3];
                    counter += quantum;
                    proc[i][4] = counter;
                    i++;
                } else {
                    //printf("less than quantum!\n");
                    proc[i][3] = counter - proc[i][4] + proc[i][3];
                    counter += proc[i][5];
                    proc[i][4] = counter;
                    proc[i][6] = 0;
                    complete++;
                    i++;
                }
            } else {
                i++;
            }
        }
    }

    return 0;
}
