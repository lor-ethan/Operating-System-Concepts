# CS441/541 Project 2

## Author(s):

Ethan Lor


## Date:

February 15, 2018


## Description:

Scheduler computes the average waiting and average turnaround times for 
a set of processes based on a selected scheduling algorithm. 


## How to build the software

The program may be built using the provided Makefile and issuing the make
command in the console or terminal while in the working directory of the program.
A successful compilation is shown below. 

$ make
gcc -o scheduler -Wall -g -O0 scheduler.c


## How to use the software

To execute the program, after the program has been successfully compiled, enter into the terminal 

$./scheduler 

along with any valid options and a text file.  The program requires at minimum a scheduling 
algorithm (-s) and a text file. The following explain the valid options along with their arguments.

* Required:  -s #
	Scheduling algorithm to use is identified by number (1 - 4).  Below describes the association
	of parameters to scheduling algorithms.
	
	-s 1		First-Come, First-Served (FCFS)
	-s 2		Shortest-Job First (SJF)	
	-s 3		Priority
	-s 4		Round-Robin (RR)

* Required for RR (-s 4) only:  -q #
	The quantum to use in the Round-Robin scheduling algorithm.  This option is not required
	for the other scheduling algorithms, and, if provided for those algorithms, is ignored. 
	The number must be a positive integer.
	
* Required:  text file
	A text file is required to be provided.  The text file must contain the number of processes
	and then the ordering of processes as they would enter the queue.  Each process must be
	on a new line and contain a process ID, CPU burst time, and priority.  A lower number will
	mean a higher priority.  The following are a template and an example of what a valid text file 
	may look like.
	
	<#> number of processes
	<#>process ID <#>CPU burst time <#>priority 
	<#>process ID <#>CPU burst time <#>priority 
	<#>process ID <#>CPU burst time <#>priority 
	...
	
	4
	4 3 7
	2 3 10
	3 5 7
	1 7 1
	
Below are valid command line options and argument examples.  Note options and arguments may
be entered in any order as long as options with their valid arguments are together.

$./scheduler -s 1 test.txt
$./scheduler test.txt -s 3
$./scheduler -s 4 test.txt -q 3
$./scheduler -q 4 -s 2 test.txt
	   

## How the software was tested

The software was tested manually and with the test suite included.  Manual testing was done to 
check mostly for invalid command line options and arguments.  Errors will be provided when options 
are missing arguments and when required options are missing.  The following are a few examples of
command line errors.

$ ./scheduler -q 3 test.txt
Error: -s option required to be specified.

$ ./scheduler -s 3
Error: a text file is required to be specified.

$ ./scheduler -s 4 test.txt
Error: -q option and argument must be specified with round robin option -s 4

$ ./scheduler -s 4 test.txt -q -3
Error: -q option requires a positive integer value.

$ ./scheduler -s 1 error.txt
Error: 'error.txt' file does not exist or could not be opened.

The program was also tested using the built in test suite to compare accuracy of computed data.
The program can be and was tested using the 'make check' command.  Additional manual tests 
were performed and provided and is discussed in the next section. 
  

## Test Suite

Additional test text files are available in the included tests directory.  These tests were all manually 
run.  The tests were design to compare the results of each algorithm compared to each other and 
how each algorithm handles corner cases specific to their particular algorithm.  An example of how 
to run these tests is shown below.

$./scheduler -s 4 tests/largeQuantum.txt -q 20 

*largeQuantum.txt 
The largeQuantum.txt file was used to test how the round robin algorithm handled scheduling processes
when the quantum was larger than any processors CPU burst time.  The conclusions were that round robin
essentially becomes a first come first server scheduler when the quantum time is larger than all the process
CPU burst times.  The computed outputs were compared and as expected round robin with a large quantum
had the same stats as the first come first served stats.

*mostlyLong.txt
The mostlyLong.txt file was use to test how each algorithm responded to a queue with mostly longer CPU
burst processes than short ones.  Results showed shortest job first has the least average wait time and 
the least average throughput time.  First come first served and priority were mixed results depending on 
order and priority.  There was a sweet spot for round robin where wait time was minimized but throughput
was sacrificed some. 

*mostlyShort.txt
The mostlyShort.txt file was used to test how each algorithm responded to a queue with mostly short CPU
burst processes than long ones.  Again shortest job first came in with the least average time in wait and
turnaround, but you sacrifice fairness by penalizing long CPU burst processes.  First come first served and
priority were once again dependent on queue order and priority and averaged out to be similar in both wait
and turnaround time.  Round robin again had a sweet spot when the quantum time was between the longest 
and shortest process CPU burst time.  This resulted in a balanced wait and turnaround average time.

*allSame.txt
The allSame.txt file was used to test how each algorithm handles have all processes with the same CPU
burst and the same priority.  As expected, first come first served, shortest job first, and priority all had 
the same stats.  Essentially when processes are all the same they become a first come first server scenario.
Round robin had the same stats as the other algorithms if the quantum was larger or equal to the CPU burst.
Smaller quantum increased both the average waiting and turnaround time.  This is expected since round robin 
will need to switch more often between processes to satisfy the quantum time.

*longPriority.txt
The longPriority.txt file was used to test how the algorithms would respond to longer processes having priority 
over the shorter processes.  First come first served was dependent on queue order and so results could be 
good or bad.  Shortest job first had the lowest average wait time and the fastest average turn around time, but 
again shortest job first penalizes longer CPU burst processes.  Priority showed the most dramatic results.  
Since priority was given to the long processes the average wait time and average turn around time were much 
longer than the other algorithms even at their worst.  Round robin did a good job of balancing out the processes 
depending on the quantum chosen.

*shortPriority.txt
The shortPriority.txt file was used to test how the algorithms would respond to short processes having priority over 
the longer processes.  First come first served again in this case depended on the ordering of the data.  In this case 
the short and long processes came in a very mixed order causing average wait and average turnaround times to 
be similar.  Shortest job first had the best average wait time and turnaround time, but again at the cost of fairness.  
Priority had good wait and turnaround times comparable to shortest job first, as with shorter jobs having higher 
priority, it essentially becomes a shortest job first algorithm.  Round robin, interestingly, had the worst stats for 
this set of data.  Turns out the short processes and long processes were very different in size and no matter what 
quantum was used there average turn around and average wait still remained high even when both stats were balanced.

This test suite shows how depending on the order, CPU burst, and priority each algorithm can produce satisfactory 
results or unsatisfactory results depending on what statistical output is desired.  One algorithm will not fit all needs.  
Each one has their implementation dependent on needs and desire.  


## Known bugs and problem areas

A known bug is no checking for repeated options and arguments.  The current program does not check if the user 
has entered repeat options and arguments.  The program will override previous correctly given and assigned options 
and parameters. The program assumes the user will enter the valid options and augments only once.  The program 
code also assumes the first non standard option encountered is the text file name.

There is no error checking of the contents of the text file.  It assumes the text file is correct and will arbritarily read in
the text file one "string" at a time and assume its a number.  This will cause errorous results as these "strings" will be
likely converted to integers and stored.  On the same note, the assumption is that all numbers in the text file are
valid.  For example processes with a CPU burst of 0 will be treated as a valid process and will be processed with the
rest of the data.  Negative numbers will also be accepted.  Assumption is the text file is accurate and correct. 











