/* 4bsd_scheduler.c
 *
 * Raymond Weiming Luo
 * CSCI 460 - Operating Systems
 * Assignment 2: CPU Scheduler
 *
 * Basic implementation of the ULE scheduling algorithm which will
 * run a simulation of multiple processes running with a max excution
 * time of 100 milliseconds per round. Schedules the work processes
 * can perform in a current and next queue. Queue is implmented as
 * a LinkedList.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include "Dispatcher.h"
#include "SchedSim.h"

struct runtimes {
  double minRuntime;
  double maxRuntime;
  double avgRuntime;
  double CPURuntime;
  int processes;
};

struct ProcessNode {
  int pid;
  double timeInQueue;
  struct ProcessNode *nextProcess;
};

struct runtimes *readyRuntime = NULL;
struct runtimes *waitRuntime = NULL;
struct runtimes *overheadRuntime = NULL;

struct ProcessNode *currentHead = NULL;
struct ProcessNode *currentTail = NULL;
struct ProcessNode *nextHead = NULL;
struct ProcessNode *nextTail = NULL;
struct ProcessNode *waitHead = NULL;
struct ProcessNode *waitTail = NULL;

/**************************************************************************************
 * Get the current time for the process in milliseconds.
 */
double getTime () {
  struct timeval time;
  gettimeofday(&time, NULL);
  
  return (double) ((time.tv_sec + time.tv_usec / 1000000.00) * 1000.00);
}

/**************************************************************************************
 * Configure the max, min and avergae time for the process states, in ready queue or
 * waiting for I/O. 
 */
void configRuntime (struct runtimes *processRuntime, struct ProcessNode *processNode) {
  double currentTime = 0.00;
  
  currentTime = getTime();
  currentTime = (currentTime - (processNode->timeInQueue));
  processRuntime->minRuntime = fmin(processRuntime->minRuntime, currentTime);
  processRuntime->maxRuntime = fmax(processRuntime->maxRuntime, currentTime);
  processRuntime->avgRuntime += currentTime;
  processRuntime->processes++;
}

/**************************************************************************************
 * Return the process that is in the waiting queue and remove it from the waiting
 * queue. Return NULL if process is not in waiting queue.
 */
struct ProcessNode* getWaitProcess (struct ProcessNode *front, int currPid) {
  struct ProcessNode* tempProcess = (struct ProcessNode *) malloc (sizeof (struct ProcessNode));

  if (tempProcess == NULL) {
    fprintf(stderr, "error : malloc getting waiting process tempProcess was unsuccessful");
    exit(EXIT_FAILURE);
  }
  
  if (front == NULL) {
    return NULL;
  }
  
  if (front->pid == currPid) {
    tempProcess = front;
    front = tempProcess->nextProcess;

    if (waitHead == NULL) {
      waitTail = NULL;
    }

    return tempProcess;
  }

  if (front->nextProcess == NULL) {
    return NULL;
  }

  tempProcess = front->nextProcess;

  while (tempProcess != NULL) {
    if (tempProcess->pid == currPid) {
      front->nextProcess = tempProcess->nextProcess;
      
      if (front->nextProcess == NULL) {
        waitTail = front;
      }
      
      return tempProcess;
    }

    if (front == front->nextProcess) {
      front->nextProcess = NULL;
      return NULL;
    }
    
    front = tempProcess;
    tempProcess = tempProcess->nextProcess;
  }
  return NULL;
}

/**************************************************************************************
 * A new process has been created.
 */
void NewProcess (int pid) {
  printf("Process %d has been created\n", pid);
  Ready(pid, 0);
}

/**************************************************************************************
 * The process with the given pid is dispatched. Swap the current queue with the next
 * queue if the current queue is empty. Remove the process out the current queue after
 * dispatch. If the pid is a process in the waiting queue, remove from the waiting queue
 * and add it to the end of the current queue.
 */
void Dispatch (int *pid) {
  double currentTime;
  struct ProcessNode *currNode = currentHead;
  struct ProcessNode *waitNode = NULL;
  struct ProcessNode *tempFrontNode = NULL;
  struct ProcessNode *tempRearNode = NULL;

  if (currentHead == NULL) {
    printf("Swapping current queue with next queue.\n");
    
    tempFrontNode = nextHead;
    tempRearNode = nextTail;
    nextHead = currentHead;
    nextTail = currentTail;
    currentHead = tempFrontNode;
    currentTail = tempRearNode;
    currNode = currentHead;

    if (currentHead == NULL) {
      printf("Both queues are empty.\n");
      return;
    }
  }
  
  *pid = currNode->pid;
  
  printf("Process %d dispatched\n", *pid);
  
  if (currentHead == currentTail) {
    currentHead = currentTail = NULL;
  } else {
    currentHead = currentHead->nextProcess;
  }

  waitNode = getWaitProcess(waitHead, *pid);

  if (waitNode != NULL && (waitNode->timeInQueue) != 0.00) {
    configRuntime(waitRuntime, waitNode);
    free(waitNode);
  }

  configRuntime(readyRuntime, currNode);
  free(currNode);
}

/**************************************************************************************
 * If process is in the waiting queue, update the waiting time of the process. If the
 * process is not in any queues, add it to the end of the current queue. Otherwise, 
 * add it to the end of the next queue.
 */
void Ready (int pid, int CPUtimeUsed) {
  struct ProcessNode* waitProcess = waitHead;
  struct ProcessNode* currNode = (struct ProcessNode *) malloc (sizeof (struct ProcessNode));

  if (currNode == NULL) {
    fprintf(stderr, "error : malloc ready currNode  was unsuccessful");
    exit(EXIT_FAILURE);
  }
  
  currNode->pid = pid;
  currNode->nextProcess = NULL;
  currNode->timeInQueue = getTime();
  overheadRuntime->CPURuntime += CPUtimeUsed;

  while (waitProcess != NULL) {
    if (waitProcess == waitProcess->nextProcess) {
      waitProcess->nextProcess = NULL;
      break;
    }
    else if (waitProcess->pid == pid) {
      waitProcess->timeInQueue = getTime();
    }

    waitProcess = waitProcess->nextProcess;
  }

  if (CPUtimeUsed < 100) {
    if (currentHead == NULL && currentTail == NULL) {
      currentHead = currentTail = currNode;
      printf("Process %d in ready queue at %d msec\n", pid, CPUtimeUsed);
      return;
    }
    currentTail->nextProcess = currNode;
    currentTail = currNode;
    
    printf("Process %d in ready queue at %d msec\n", pid, CPUtimeUsed);
  } else {
    if (nextHead == NULL && nextTail == NULL) {
      nextHead = nextTail = currNode;
      printf("Process %d in ready queue at %d msec\n", pid, CPUtimeUsed);
      return;
    }
    nextTail->nextProcess = currNode;
    nextTail = currNode;
    
    printf("Process %d in ready queue at %d msec\n", pid, CPUtimeUsed);
  }
}
/**************************************************************************************
 * Process is added to the end of the waiting queue.
 */
void Waiting (int pid) {
  struct ProcessNode* currNode = (struct ProcessNode *) malloc (sizeof (struct ProcessNode));

  if (currNode == NULL) {
    fprintf(stderr, "error : malloc waiting currNode  was unsuccessful");
    exit(EXIT_FAILURE);
  }

  currNode->pid = pid;
  currNode->nextProcess = NULL;
  currNode->timeInQueue = 0.00;
  
  if (waitHead == NULL && waitTail == NULL) {
    waitHead = waitTail = currNode;
    return;
  }
  
  waitTail->nextProcess = currNode;
  waitTail = currNode;
  
  printf("Process %d waiting\n", pid);
}

/**************************************************************************************
 * Process has been terminated.
 */
void Terminate (int pid) {
  printf("Process %d terminated\n", pid);
}

/**************************************************************************************
 * Main function, initialize the runtime structs and run the simulation. Output the
 * runtimes after the simulation has ended.
 */
int main () {
  readyRuntime = (struct runtimes *) malloc (sizeof (struct runtimes));
  waitRuntime = (struct runtimes *) malloc (sizeof (struct runtimes));
  overheadRuntime = (struct runtimes *) malloc (sizeof (struct runtimes));

  if (readyRuntime == NULL || waitRuntime == NULL || overheadRuntime == NULL) {
    fprintf(stderr, "error : malloc main was unsuccessful");
    exit(EXIT_FAILURE);
  }
 
  readyRuntime->minRuntime = 999.99;
  readyRuntime->maxRuntime = 0.00;
  readyRuntime->avgRuntime = 0.00;
  readyRuntime->processes = 0.00;
  readyRuntime->CPURuntime = 0.00;

  waitRuntime->minRuntime = 999.99;
  waitRuntime->maxRuntime = 0.00;
  waitRuntime->avgRuntime = 0.00;
  waitRuntime->processes = 0.00;
  waitRuntime->CPURuntime = 0.00;
  
  overheadRuntime->minRuntime = 999.99;
  overheadRuntime->maxRuntime = 0.00;
  overheadRuntime->avgRuntime = 0.00;
  overheadRuntime->processes = 0.00;
  overheadRuntime->CPURuntime = 0.00;
  
  overheadRuntime->minRuntime = getTime();
  Simulate(1000, 100);
  overheadRuntime->maxRuntime = getTime();

  printf("\n============================================\n"
         "Process spent in ready queue\n"
         "Minimum : %f\n"
         "Maximum : %f\n"
         "Average : %f\n"
         "\n",
         readyRuntime->minRuntime,
         readyRuntime->maxRuntime,
         (readyRuntime->avgRuntime / readyRuntime->processes));

  printf("Process spent between I/O and next dispatch\n"
         "Minimum : %f\n"
         "Maximum : %f\n"
         "Average : %f\n"
         "\n",
         waitRuntime->minRuntime,
         waitRuntime->maxRuntime,
         (waitRuntime->avgRuntime / waitRuntime->processes));

  printf("Proportion of time spent on overheads\n"
         "Overhead : %f\n"
         "============================================\n",
         ((overheadRuntime->maxRuntime - overheadRuntime->minRuntime) - overheadRuntime->CPURuntime));
}
