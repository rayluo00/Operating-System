/* Process1.c
 *
 * Author: Raymond Weiming Luo
 * CSCI 460 - Operating Systems
 * Assignment 1: Shared Memory
 *
 * Process1 will create a shared memory and store the data of the user input for Process2
 * to access and used later.
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
 * Process1 will fork a child process to get a number from the user in stdin. The child
 * will then create a shared memory to store the input that Process2 can later access.
 * Child process will sleep to wait until Process2 finishes using the shared memory before
 * closing.
 *
 */
void main () {
  int childPid;
  int inputNum;
  int *shareMem;
  int *memory;
  int shareMemId;
  key_t key;

  // Key to access shared memory.
  key = 8888;
  
  printf("%s", "Input a number : ");
  scanf("%d", &inputNum);
  
  childPid = fork();

  // Fork Error
  if (childPid < 0) {
    perror ("fork");
    exit(EXIT_FAILURE);
  }
  // Child
  else if (childPid == 0) {
    printf("%s | %s : %d\n", "Child process 1 running...", "Process 1 Input Number", inputNum);
    
    if ((shareMemId = shmget (key, 27, IPC_CREAT | 0666)) < 0) {
      perror ("shmget");
      exit(EXIT_FAILURE);
    }

    if ((shareMem = shmat (shareMemId, NULL, 0)) == (int *) -1) {
      perror ("shmat");
      exit(EXIT_FAILURE);
    }
    
    *shareMem = inputNum;
    *memory = inputNum;

    // Sleep until the data in the shared memory has been updated.
    while (*shareMem != (inputNum%26)) {
      printf("%s | %d\n", "Child 1 sleeping...", *shareMem);
      sleep(3);
    }

    printf("%s | %d\n", "Child 1 awake...", *shareMem);
  }
  // Parent
  else if (childPid > 0) {
    printf("%s\n","Parent process 1 running...");
    wait(&childPid);
    printf("%s\n","Parent process 1 finished waiting.");
  }
}
