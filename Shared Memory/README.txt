Weiming Raymond Luo
CSCI 460 - Operating Systems
Assignment 1

This assignment is to work with two process that will fork a child process to
compute a simple calculation. Process A obtains the input which will use shared
memory to pass it to Process B then goes to sleep. From Process B, it will
access the shared memory to recieve the input data and perform ([input number] % 26).
After the calculation, the child process of B will pass the data to the parent process
to print out the final computation. The the child process of B will terminate and
the parent process outputs the final number.

To run the program, the Makefile can compile the required programs (Process1.c and  Process2.c)
using the command : "make". To run the program, first run p1 (Process1.c) first as it
will read for user input. After you provide p1 an input, it will sleep and then you can run
p2 (Process2.c).

LIST OF FILES :
Process1.c
Process2.c
Makefile
README.txt
