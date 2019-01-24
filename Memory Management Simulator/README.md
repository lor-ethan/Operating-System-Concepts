# CS 441/541 Project 6

## Authors:

* Ethan Lor
* Tyler Durr

## Date

April 8, 2018

## Description

The program implements a memory management simulator for page replacement and will report the number of page faults for a specified number of frames or a default range of frames.

## How to build the software

The program may be built using the provided Makefile. The Makefile will build the program and all other dependencies. To build the program, use `make` as shown in the following example (output included).

### Output of `make`

```
gcc -o pagefault -Wall -g -O0 pagefault.c
```

## How to use the software

After `make`ing the program, call `./pagefault` with an input file name and the following optional argument:

### Optional: `-f <number of frames>`

The number of frames to use in the algorithm analysis. Valid number of frames range from 1 to 7, inclusive. If this parameter is not provided then the program will default to running the full range of frames 1 to 7 on each algorithm.

Command line arguments may occur in any order in the command line, but the program will only handle
processing the first file encountered. Any other non valid arguments or files will be ignored after receiving a valid option and file.

## How the software was tested

The program was run using `make check`, which will put the program through a battery of 56 tests. We also used five of our own tests, detailed below, for more specific tests.

### Test Suite

#### `./pagefault ./given-tests/level1.txt -f 3 ./given-tests/level2.txt`

Tests whether our code can handle extraneous files being passed in. Only the first file should be used; subsequent files are discarded. (Additionally, this shows that we can read in the `-f` argument after a file argument.)

#### `./pagefault -f ./given-tests/level1.txt`, `./pagefault -f 2.5 ./given-tests/level1.txt`, and `./pagefault -f 300 ./given-tests/level1.txt`

These test that our code will successfully fail when an invalid value is passed for `-f`.

#### `./pagefault ./tests/test1.txt`

This will test one read on every page from 0 to 99. The benefit of this test is that there will always be 100 page faults, making it easy to verify the results (100 page faults).

##### Expected output

```
Num. Frames : All
Ref. File   : ./tests/test1.txt
-------------------------------
Reference String:
  0:R,  1:R,  2:R,  3:R,  4:R,  5:R,  6:R,  7:R,  8:R,  9:R,
 10:R, 11:R, 12:R, 13:R, 14:R, 15:R, 16:R, 17:R, 18:R, 19:R,
 20:R, 21:R, 22:R, 23:R, 24:R, 25:R, 26:R, 27:R, 28:R, 29:R,
 30:R, 31:R, 32:R, 33:R, 34:R, 35:R, 36:R, 37:R, 38:R, 39:R,
 40:R, 41:R, 42:R, 43:R, 44:R, 45:R, 46:R, 47:R, 48:R, 49:R,
 50:R, 51:R, 52:R, 53:R, 54:R, 55:R, 56:R, 57:R, 58:R, 59:R,
 60:R, 61:R, 62:R, 63:R, 64:R, 65:R, 66:R, 67:R, 68:R, 69:R,
 70:R, 71:R, 72:R, 73:R, 74:R, 75:R, 76:R, 77:R, 78:R, 79:R,
 80:R, 81:R, 82:R, 83:R, 84:R, 85:R, 86:R, 87:R, 88:R, 89:R,
 90:R, 91:R, 92:R, 93:R, 94:R, 95:R, 96:R, 97:R, 98:R, 99:R
-------------------------------
####################################################
#     Frames    Opt.    FIFO     LRU      SC     ESC
#          1     100     100     100     100     100
#          2     100     100     100     100     100
#          3     100     100     100     100     100
#          4     100     100     100     100     100
#          5     100     100     100     100     100
#          6     100     100     100     100     100
#          7     100     100     100     100     100
####################################################
```

#### `./pagefault ./tests/test2.txt`

This will test one write on every page from 0 to 99. This ensures that our write behavior works as expected.

##### Expected output

```
Num. Frames : All
Ref. File   : ./tests/test2.txt
-------------------------------
Reference String:
  0:W,  1:W,  2:W,  3:W,  4:W,  5:W,  6:W,  7:W,  8:W,  9:W,
 10:W, 11:W, 12:W, 13:W, 14:W, 15:W, 16:W, 17:W, 18:W, 19:W,
 20:W, 21:W, 22:W, 23:W, 24:W, 25:W, 26:W, 27:W, 28:W, 29:W,
 30:W, 31:W, 32:W, 33:W, 34:W, 35:W, 36:W, 37:W, 38:W, 39:W,
 40:W, 41:W, 42:W, 43:W, 44:W, 45:W, 46:W, 47:W, 48:W, 49:W,
 50:W, 51:W, 52:W, 53:W, 54:W, 55:W, 56:W, 57:W, 58:W, 59:W,
 60:W, 61:W, 62:W, 63:W, 64:W, 65:W, 66:W, 67:W, 68:W, 69:W,
 70:W, 71:W, 72:W, 73:W, 74:W, 75:W, 76:W, 77:W, 78:W, 79:W,
 80:W, 81:W, 82:W, 83:W, 84:W, 85:W, 86:W, 87:W, 88:W, 89:W,
 90:W, 91:W, 92:W, 93:W, 94:W, 95:W, 96:W, 97:W, 98:W, 99:W
-------------------------------
####################################################
#     Frames    Opt.    FIFO     LRU      SC     ESC
#          1     100     100     100     100     100
#          2     100     100     100     100     100
#          3     100     100     100     100     100
#          4     100     100     100     100     100
#          5     100     100     100     100     100
#          6     100     100     100     100     100
#          7     100     100     100     100     100
####################################################
```

#### `./pagefault -f 7 ./tests/test3.txt`

This test is specifically for the enhanced second chance algorithm. It contains a write to each page, one to seven, such that ESC fills all seven frames with pages whose bits are all `1, 1`. Then, the eighth write to page eight will cause ESC to run through all four loops—`0, 0`, `0, 1` (setting all second-chance bits to 0), `0, 0`, and `0, 1`. By adding this line immediately before the return from `LRU_ESC()`:

`printf("frame 0: page=%i\n", frames[0].page);`

You can ensure that the first frame is being set to page 8.

##### Expected output (including additional line of code)

```
Num. Frames :  7
Ref. File   : ./tests/test3.txt
-------------------------------
Reference String:
  1:W,  2:W,  3:W,  4:W,  5:W,  6:W,  7:W,  8:W
-------------------------------
####################################################
#     Frames    Opt.    FIFO     LRU      SC     ESC
frame 0: page=8
#          7       8       8       8       8       8
####################################################
```

## Known bugs and problem areas

We tested our code very thoroughly and we could not find any bugs. As with any other program, there is a chance that we've missed a corner case where a bug exists; that being said, we strove to weed out all corner cases we could conjure up and correct any bugs that were found.
