# CS441/541 C Lab - Part 1
## Program 2 - cmdline

## Author(s):

Ethan Lor


## Date:

February 5, 2018


## Description:

Program parses a command line for valid options and arguments.


## How to build the software:

Use Makefile provided to compile the cmdline.c file. Run the command make. On success, 
the terminal will print to standard output the following line. 
gcc -o cmdline cmdline.c -Wall -g -O0


## How to use the software:

Execute the program passing into the command line options and arguments.
Valid options and arguments are listed below.

*Required: -n #
           Number of iterations. Parameter must be a positive integer greater than 0.
*Optional: -d
           Enable debugging output in the main program. Default: Disabled.
*Optional: -stage NAME
           Run only the stage identified by the string NAME. Default: Runs all stages.

Options may be passed into the command line in any order. Program will report any
errors to standard output. The following displays a valid command line with options and
arguments along with the successful standard output display.

$ ./cmd -n 20 -d -stage FOO
#-----------------------
# Iterations : 20
# Debugging  : Enabled
# Stage(s)   : FOO
#-----------------------


## How the software was tested:

The program was tested with none to all options and arguments in the command line. 
Each option was tested with and with out their argument/s. Options (with and
with out their arguments) were also tested in all possible order and combinations.
Invalid options and arguments were also tested alone and in combination with the 
valid options and arguments.

Included with the program is a test file with test cases. To test the program call
the command:
$ make
and then call the command: 
$ make check

The make check command will run sample test cases and report to standard output
any errors if the program outputs do not match the expected outputs. Below is a 
sample of a successful make check test.

$ make check
By default check will run all levels.
Running test 1: ./cmdline                     
Running test 2: ./cmdline -n 20               
Running test 3: ./cmdline -d -n 16            
Running test 4: ./cmdline -n 234 -d           
Running test 5: ./cmdline -n 34 -stages       
Running test 6: ./cmdline -n 42 -stage FOO    
Running test 7: ./cmdline -n 142 -d -stage FOO
Running test 8: ./cmdline -d -stage BIP -n 101
Running test 9: ./cmdline -n                  
Running test 10: ./cmdline -n -20              
Running test 11: ./cmdline -n 0                
Running test 12: ./cmdline -n 24 -stage        
Autograde score: 12 / 12

Options and arguments may also be provided manually while executing the program.
The following samples shows successful and unsuccessful manual command line executions.

$ ./cmdline -n 10
#-----------------------
# Iterations : 10
# Debugging  : Disabled (Default)
# Stage(s)   : All (Default)
#-----------------------
$ ./cmdline -n 10 -d
#-----------------------
# Iterations : 10
# Debugging  : Enabled
# Stage(s)   : All (Default)
#-----------------------
$ ./cmdline -n 10 -d -stage FOO
#-----------------------
# Iterations : 10
# Debugging  : Enabled
# Stage(s)   : FOO
#-----------------------
  
$ ./cmdline -n 10 -d -stage
Error: Not enough arguments to the -stage option.
$ ./cmdline -stage FOO
Error: -n option required to be specified.
$ ./cmdline -stage FOO -n
Error: Not enough arguments to the -n option.


## Known bugs and problem areas:
Errors printed to standard output may not be the most precise in what caused 
the resulting error. When certain options entered without a required argument
is followed by correct options and arguments, the error reported is not as precise
as it could be. See example below:

$ ./cmdline -stage -n 20 -d
Error: -stage option requires a string NAME

A more precise error report for the above command line should be:
Error: Not enough arguments to the -stage option.

To correct for this the entire command line must be parsed before error 
checking is done. The current program parses each command line options
and arguments in a left to right fashion and handles errors as they occur.
