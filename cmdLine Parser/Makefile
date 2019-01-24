#
# [YOUR NAME]
# [DATE LAST MODIFIED]
#
CC=gcc
CFLAGS=-Wall -g -O0

#
# List all of the binary programs you want to build here
# Separate each program with a single space
#
PROGS=cmdline

#
# First target: all
# Compile all programs
#
all: $(PROGS)

#
# Compile Program 1
#
cmdline: cmdline.c
	$(CC) -o cmdline cmdline.c $(CFLAGS) 


#
# Cleanup the files that we have created
#
clean:
	$(RM) $(PROGS)
	$(RM) -rf *.o *.dSYM
	$(RM) test_out

#
# Tests
#
include given-tests/Makefile
