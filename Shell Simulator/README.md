# CS 441/541 Project 3

## Author(s):
* Ethan Lor
* Tyler Durr

## Date:
February 27, 2018

## Description:
The program `mysh.c` simulates a full functioning shell along with custom built-in functions that mimic similar functions to their native Linux shell counterparts.

## How to build the software
The program may be built using the provided Makefile. The Makefile will build the program and all other dependencies. To build the program, use `make` as shown in the following example.

### Make example and expected output

```
$ make
gcc -c -o support.o support.c -Wall -g
gcc -o mysh mysh.c support.o -Wall -g
```

## How to use the software
The program simulates a native Linux shell and is used similar to one. The `mysh.c` program can be run in either Interactive Mode or Batch Mode.  Each of the two modes are explained below.

### Interactive mode
To enter interactive mode, launch the executable `mysh` after compiling the program. In interactive mode the shell simulator program will display the prompt `mysh$`. The command entered after the prompt will be interpreted by the shell. Unless explicitly indicated by the job separator, the jobs contained in the command are executed sequentially and in the order typed on the command line. The prompt is returned when all of the jobs have completed. To exit, type `exit` in the command like or press 'Ctrl-D'. An example of using the interactive shell is provided below.

```
mysh$ date
Wed Feb 17 10:00:00 CDT 2018
mysh$ pwd
/home/student/project2/part2
mysh$ ls tests
file1.txt file.txt
mysh$ exit
-------------------------------
Total number of jobs = 3
Total number of jobs in history = 4
Total number of jobs in background = 0
```

### Batch mode
The program can be run in Batch Mode by specifying one or more batch files to the shell on the
command line.  The batch file should contain a list of commands, one per line, that are to be executed by `mysh`.  If multiple batch files are entered, the files will be executed sequentially in the order they are typed on the command line.  Examples of the bash shell interaction are shown below.

#### Single-file batch mode

```
[student@CS441 ~]$ cat tests/file1.txt
date
pwd
ls tests
[student@CS441 ~]$ ./mysh tests/file1.txt
Wed Feb 17 10:00:00 CDT 2018
/home/student/project2/part2
file1.txt file.txt
-------------------------------
Total number of jobs = 3
Total number of jobs in history = 3
Total number of jobs in background = 0
```

#### Multiple-file batch mode

```
[student@CS441 ~]$ ./mysh tests/file1.txt tests/file1.txt
Wed Feb 17 10:00:00 CDT 2018
/home/student/project2/part2
file1.txt file.txt
Wed Feb 17 10:00:00 CDT 2018
/home/student/project2/part2
file1.txt file.txt
-------------------------------
Total number of jobs = 6
Total number of jobs in history = 6
Total number of jobs in background = 0
[student@CS441 ~]$
```

### Concurrent execution of multiple jobs
In both interactive and batch mode, a command/job may be run in the background. Commands/jobs that are to be run in the background are followed by an `&`. Foreground jobs are followed instead by a `;` to separate them in a multiple job execution, or by nothing if the command is last on that line. Examples of multiple job executions are shown below.

```
mysh$ sleep 4 ; sleep 5 ; sleep 3
mysh$
mysh$ sleep 4 & /bin/sleep 5 & /bin/sleep 3 & date &
Wed Sept 17 10:00:00 CDT 2017
mysh$ sleep 4 & jobs ; sleep 3
[1] Running sleep 4
mysh$
```

### Built-in commands
The program supports five built-in commands: `jobs`, `history`, `wait`, `fg`, and `exit`.  These commands mimic the respective native commands found in a Linux shell. Each is detailed below.

#### `jobs`
`jobs` will display a list of the jobs that are currently running or have completed since the last time `jobs` was called. If a job has finished, it will be displayed as done only once when `jobs` is executed and will not appear in subsequent calls to `jobs`. `jobs` will not display processes which failed to run or returned an error code upon completion. An example is given below.

```
mysh$ /bin/sleep 5 & sleep 4 & sleep 3 &
mysh$ jobs
[4]	Running	 /bin/sleep 5
[5]	Running	 sleep 4
[6]	Running	 sleep 3
mysh$ jobs
[4]	Done	 /bin/sleep 5
[5]	Done	 sleep 4
[6]	Done	 sleep 3
mysh$ jobs
mysh$
```

#### `history`
The `history` command will display the full history of job commands typed into the shell. The history command displays all jobs executed by the shell (including the erroneous ones) from the earliest command to the latest command (the current call to history). An example is given below.

```
mysh$ history
1 /bin/date
2 /bin/pwd
3 /bin/ls tests
4 /bin/sleep 5 &
5 sleep 4 &
6 sleep 3 &
7 ./doesntexist
8 ./returnedfailure
9 jobs
10 history
mysh$
```

#### `wait`
The `wait` command will wait for all currently backgrounded jobs to complete. If there are no backgrounded jobs, then the command returns immediately.

#### `fg`
The foreground command will wait for a specific currently backgrounded job. If there are no backgrounded jobs, then the command returns an error stating there are no background jobs.

`fg` takes zero or one arguments. If there are zero arguments, then the latest backgrounded process is brought into the foreground and the shell will not return until the job has completed. If there is an argument, it will be the job ID of the job to be brought into the foreground, as listed by `jobs`. If the job has already completed, an error message reports the job has already completed and the job is precluded from being displayed in subsequent calls to `jobs`.

#### `exit`
The shell terminates when it sees the `exit` command, reaches the end of the input stream, or the user types 'Ctrl-D'. If a background process is still running when the shell exits, the shell will wait for that process to complete before exiting. The shell will print a message indicating the number of jobs the shell is waiting on. Total number of jobs, jobs in history, and jobs in background are printed to stdout upon completion.

## How the software was tested

### `./hello < helloin.txt > helloout.txt ; ./hello` (assuming that helloin.txt does not exist)

#### Test

This tests what happens to our redirection when helloin.txt doesn't exist. (The second call to `./hello` is to ensure that redirects disappear after each command.)

#### Expected

* An error displaying this issue will be printed to `stderr` and the command does not run.

#### Actual

* An error displaying this issue is printed to `stderr` and the command does not run.

### `sleep 10 & sleep 5 &`, then `fg`

#### Test

This test backgrounds two processes, `sleep 10` and `sleep 5`, then calls `fg` to see which process is foregrounded.

#### Expected

* The `sleep 5` process will be foregrounded.

#### Actual

* The `sleep 5` process is foregrounded. More generally, the most recently backgrounded process is foregrounded.

### `sleep 10 &`, wait 10 seconds, then `fg <job #>`, then `jobs`

#### Test
Background a process and wait for it to complete, then try to bring it to the foreground with `fg <job #>`. After that, call jobs to view a list of jobs.

#### Expected

* Because the job previously completed, `fg <job #>` will display a message indicating that the job has previously completed. This call will also preclude that job from being displayed in any subsequent jobs output.
* `jobs` will not show the foregrounded job in its output because of the previous call to `fg <job #>`.

#### Actual

* Because the job previously completed, `fg <job #>` displays a message indicating that the job has previously completed. This call also precludes that job from being displayed in any subsequent jobs output.
* `jobs` does not show the foregrounded job in its output because of the previous call to `fg <job #>`.

### `tyler & ethan ;`

#### Test

Our code should be able to handle not just one bogus command, but multiple bogus commands at once, both in the background and the foreground. This tests just that.

#### Expected

* Two errors should print out—one for each of the commands on this line.

#### Actual

* Two errors print out—one for each of the commands on this line.

### `./hello < helloin.txt > helloout.txt ; ./hello < helloin2.txt > helloout2.txt ;`

#### Test

Each command can be redirected to different places. Here, we test the ability of our code to use redirection on subsequent commands from the same line.

#### Expected

* The first `./hello` will read from helloin.txt and output to helloout.txt.
* The second `./hello` will read from helloin2.txt and output to helloout2.txt.

#### Actual

* The first `./hello` reads from helloin.txt and outputs to helloout.txt.
* The second `./hello` reads from helloin2.txt and outputs to helloout2.txt.

### `./hello < notreal.txt > helloout.txt ; ./hello < helloin.txt > helloout.txt`

#### Test

If the first input redirection fails, subsequent redirections should still succeed. We use "notreal.txt" as a filename which does not exist.

#### Expected

* The first `./hello` will fail with an error to stderr.
* The second `./hello` will be unaffected by the first's failure and should continue on as normal.

#### Actual

* The first `./hello` fails with an error to stderr.
* The second `./hello` is unaffected by the first's failure and continues on as normal.

### `sleep 5 & sleep 10 &`, then `wait`

#### Test

When `wait` is called, all backgrounded tasks are waited on for completion, after which wait returns to mysh.

#### Expected

* The call to `wait` will block for approximately 10 seconds, e.g., the length of `sleep 10 &`, the longest-lasting backgrounded command.

#### Actual

* The call to `wait` blocks for approximately 10 seconds until `sleep 10 &` is complete.

### `sleep 5 & sleep 10 &`, then `exit`

#### Test

`exit`, like `wait`, waits until all backgrounded tasks complete.

#### Expected

* The call to `exit` will block for about 10 seconds (the length of `sleep 10 &`), print information about job totals, then exit the program.

#### Actual

* The call to `exit` blocks for about 10 seconds, prints information about job totals, then exits the program.

### `tyler & ethan &`, then `jobs`, then `history`

#### Test

Both `tyler` and `ethan` are commands which do not exist. This means that `jobs` should not display them. However, they should still display in `history`.

#### Expected

* `jobs` will display nothing.
* `history` will display the following:
```
	1 tyler &
	2 ethan &
	3 jobs
	4 history
```

#### Actual

* `jobs` displays nothing.
* `history` displays the output listed above.

## Known bugs and problem areas

We tested our code very thoroughly and we could not find any bugs. When the assignment was unclear on any specific test case, we considered the functionality of the native bash prompt to be the expected functionality. As with any other program, there is a chance that we've missed a corner case where a bug exists; that being said, we strove to weed out all corner cases we could conjure up.
