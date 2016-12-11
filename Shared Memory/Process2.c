/* Process2.c
 *
 * Raymond Weiming Luo
 * CSCI 460 : Operating Systems
 * Assignment 1: Shared Memory
 *
 * Process2 retrieve data from shared memory that was placed from Process1 to perform
 * a simple math computation.
 *
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>

/*****************************************************************************************
 * Process2 will fork a child process to retrieve the input number from the child process 
 * in Process1 via shared memory. Then perform [Input Number] % 26, to pipe the result to
 * the parent process. The parent process will then print the result. 
 *
 */

void main () {
  int childPid;
  int shareMemId;
  int val;
  key_t key;
  int *shareMem;
  int *mem;
  int fd[2];

  // Key to access shared memory
  key = 8888;

  if (pipe(fd)) {
    fprintf(stderr, "error : pipe failed.");
    exit (EXIT_FAILURE);
  }
    
  childPid = fork();

  // Fork Error
  if (childPid < 0) {
    perror ("fork");
    exit(EXIT_FAILURE);
  }
  // Child
  else if (childPid == 0) {
    close(fd[0]);
    if ((shareMemId = shmget (key, 27, 0666)) < 0) {
      perror ("shmget");
      exit (EXIT_FAILURE);
    }

    // Recieve the shared memory created in Process1.c
    if ((shareMem = shmat (shareMemId, NULL, 0)) == (int *) -1) {
      perror ("shmat");
      exit (EXIT_FAILURE);
    }
    
    printf("%s | %s : %d\n", "Child process 2 running...", "Process 2 Number (Before)", *shareMem);

    *shareMem = *shareMem % 26;
    
    write(fd[1], shareMem, sizeof(shareMem));
    close(fd[1]);
  }
  // Parent
  else if (childPid > 0) {
    close(fd[1]);
    printf("%s\n","Parent process 2 running...");
    wait(&childPid);
    read(fd[0], &val, sizeof(val));
    printf("%s | %s : %d\n", "Parent process 2 running...", "Process 2 Number (After)", val);
    close(fd[0]);
  }
}
