# CS 441/541 Project 5

## Authors

* Tyler Durr
* Ethan Lor

## Date

March 27, 2018

## Description

An election polling station services a steady stream of voters throughout the day. The polling station has a fixed number of voting booths available. There are three types of registered voters:

* Republicans,
* Democrats, and
* independents.

A new voter law has restricted the types of registered voters that can vote at the same time. Republicans and Democrats are never allowed to vote at the same time, but independents can vote with either Republicans or Democrats (in the hope that doing so will turn them to one side or the other). Our program creates the requisite number of Republican, Democrat, and independent voters as threads and uses semaphores to synchronize their actions in the polling station. Each voter thread identifies itself by political party and thread identifier whenever its state changes via calls to `printf`.

## How to build the software

The program may be built using the provided Makefile. The Makefile will build the program and all other dependencies. To build the program, use `make` as shown in the following example (output included).

```
make
gcc -Wall -g -O0 -I../lib   -c -o ../lib/semaphore_support.o ../lib/semaphore_support.c
gcc -o finicky-voter finicky-voter.c ../lib/semaphore_support.o -Wall -g -O0 -I../lib -pthread
```

## How to use the software

After `make`ing the program, call `./finicky-voter` with zero to four whole numbers in the following order:

1. Number of voting booths at the polling station. The default value is 10.
2. Number of Republicans. The default value is 5.
3. Number of Democrats. The default value is 5.
4. Number of independents. The default value is 5.

As mentioned above, all four of these arguments are optional, but if specified, must be positive integers.

## Software tests

### `./finicky-voter notanumber notanumber2 40 2` and `./finicky-voter -3 10 40 2`

These tests confirm that invalid input cannot be passed to the program. The program prints that the rightmost invalid argument is invalid (`notanumber2` in the first case and `-3` in the second) and then reminds the user of the proper input format.

#### Output

```
./finicky-voter notanumber notanumber2 40 2
Argument "notanumber2" is not a positive integer.
Failed to parse arguments. A valid call to this program is of the form
./finicky-voter <booths> <republicans> <democrats> <independents>
where all four arguments are optional positive integers.
```

### `./finicky-voter 3 30 30 30`

Having many voters and few booths rigorously tests our line and booth queues. With this many voters, it's almost guaranteed that there will be a queue at some point.

### `./finicky-voter 30`

When fewer than four arguments are passed to this program, the default values should be assigned. We test whether our default values of five republicans, five democrats, and five independents are functioning. Additionally, this test checks that our program still functions when the booth queue is never used, as there are enough booths for the maximum number of voters—five of a party and five independents—to vote simultaneously.

### `./finicky-voter 5 5 5 40`

With a lot of independents, there is a great chance that there might be *only* independents voting at a given time. Testing this ensures that the transfer of the voting area between Republicans and Democrats operates as expected.

## Known bugs and problem areas

We tested our code very thoroughly and we could not find any bugs. As with any other program, there is a chance that we've missed a corner case where a bug exists; that being said, we strove to weed out all corner cases we could conjure up and correct any bugs that were found.

## Questions

### 1. Describe your solution to the problem (in words, not code).

`main` will initialize all semaphores (except each voter's personal semaphore) and initiate the creation of all voter threads. Each voter…

1. (*Wait for the polling station to open.*) Wait on a barrier—the front door of the polling station, in effect.
	- After two seconds to allow all voters to get created and wait on the barrier, `main` will signal the barrier as many times as there are voters, in effect, opening the polling station.
2. (*Enter the polling station.*) Sign in for a random number of microseconds between 0 and 50,000, inclusive.
3. Add themselves to the line queue.
4. There are two possibilities.
	- If this voter is first in line and, for Republicans and Democrats, the other party isn't in the voting area, immediately leave the line queue and advance to the booth queue.
	- In all other cases, wait on your own semaphore. You may be woken up when you're at the front of the line queue, at which point you will need to verify that you can advance to the voting area before actually doing so.
5. (*Wait on a booth.*) Add themselves to the booth queue.
6. There are two possibilities.
	- If this voter is first in line or there is an empty booth, immediately leave the booth queue.
	- In all other cases, wait on your own semaphore.
7. Take a booth.
8. (*Vote.*) Vote for a random number of microseconds between 0 and 100,000, inclusive.
9. Leave your booth.
10. (*Leave the polling station.*) If the booth queue has voters waiting in it, signal the first person in line to move to the booth you just left.
11. If this voter is a Republican or a Democrat and they are the last from their party to leave the voting area, signal the first person in the line queue to advance to the voting area.

### 2. Describe how your solution meets the goals of this part of the project.

#### How does your solution avoid deadlock?

We avoid deadlock by using two queues and only waking up one thread at a time from each queue. All global variables and critical sections are mutexed with their own mutexes. When a thread/voter is woken up in the line queue, it will check to see if it can progress to the booth queue before doing so. If it cannot progress, the thread/voter will put itself back to sleep. Essentially, we are breaking hold-and-wait. There are multiple checks each thread/voter has to do before proceeding to the next step. If all conditions (“all resources”) are not met, the thread cannot proceed.

#### How does your solution prevent Republicans and Democrats from waiting on a voting booth at the same time?

We utilized two queues and a variable of the party that is in the voting area.  We have one queue for waiting outside the voting area (`line_queue`) and another one for waiting inside the voting area (`booth_queue`). Before moving from the line queue into the voting area and potentially being queued inside the voting area, a Republican or Democrat will be signaled by either the person in front of them or the last Republican/Democrat leaving the voting area. The signaled thread will need to check the party in the voting area. If the opposite party is not in the voting area, then it will proceed to the voting area, where it will “wait” on a voting booth, queueing itself if necessary.

#### How is your solution “fair”? How are you defining “fairness” for this problem?

“Fairness” is defined in this problem as first-in first-out. Each thread/voter proceeds through the “voting process” in the order they line up (sign in and queue) outside the voting area.

##### Is it fair for independent voters to cut in line ahead of other party-affiliated voters waiting for a booth?

**No.** No voters are allowed to cut the queue. This is axiomatic for a first-in first-out queue.

##### What happens if a long line of Democrats start waiting to vote and a Republican shows up late? Should the late Republican be able to vote before all of the Democrats have finished?

**No.** No voters are allowed to cut the queue. This is axiomatic for a first-in first-out queue.

#### How does your solution prevent starvation? How are you defining “starvation” for this problem?

We consider a thread to be “starved” when they cease permanently to move through the polling station. Then, to prevent this starvation, the voter at the front of the line queue is signaled when…

* the voter who was previously in front of them advances to the booth queue, or
* the last Republican/Democrat leaves the voting area.
