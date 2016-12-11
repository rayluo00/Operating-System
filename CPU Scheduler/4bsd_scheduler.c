/* 4bsd_scheduler.c
 *
 * Raymond Weiming Luo
 * CSCI 460 - Operating Systems
 * Assignment 2: CPU Scheduler
 *
 * This is a basic implementation on the 4BSD scheduler algorithm using
 * a heap to store the process data and a priority queue implementation.
 * The max timeslice for each process is 100 milliseconds. 
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include "Dispatcher.h"
#include "SchedSim.h"

enum Status {
  READY,
  WAIT,
  RUN
};

enum QueueType {
  PRIORITY,
  WAITING
};

typedef struct {
  int pid;
  int priority;
  double timeInQueue;
  enum Status status;
} ProcessNode;

typedef struct {
  ProcessNode *process;
  int size;
  int maxSize;
} PriorityQueue;

struct runtimes {
  int processes;
  double minRuntime;
  double avgRuntime;
  double maxRuntime;
  double CPURuntime;
};

struct runtimes* readyRuntime = NULL;
struct runtimes* waitRuntime = NULL;
struct runtimes* overheadRuntime = NULL;
PriorityQueue *pQueue;
PriorityQueue *wQueue;

/**************************************************************************************
 * Initilize the runtime and priority queue structs.
 */
void initializeStructs () {
  readyRuntime = (struct runtimes*) malloc(sizeof(struct runtimes));
  waitRuntime = (struct runtimes*) malloc(sizeof(struct runtimes));
  overheadRuntime = (struct runtimes*) malloc(sizeof(struct runtimes));

  if (readyRuntime == NULL || waitRuntime == NULL || overheadRuntime == NULL) {
    fprintf(stderr, "error : initializeStructs() malloc failed");
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

  pQueue = (PriorityQueue *) calloc(1, sizeof (PriorityQueue));
  wQueue = (PriorityQueue *) calloc(1, sizeof (PriorityQueue));
}

/**************************************************************************************
 * Get the current time for the process in milliseconds.
 */
double getCurrentTime (){
  struct timeval time;
  gettimeofday(&time, NULL);
  
  return (double) ((time.tv_sec + time.tv_usec / 1000000.0) * 1000);
}

/**************************************************************************************
 * Configure the max, min and avergae time for the process states, in ready queue or
 * waiting for I/O. 
 */
void configRuntime (struct runtimes *processRuntime, ProcessNode processNode) {
  double currentTime  = getCurrentTime();
  
  currentTime = (currentTime - (processNode.timeInQueue));
  processRuntime->minRuntime = fmin(processRuntime->minRuntime, currentTime);
  processRuntime->maxRuntime = fmax(processRuntime->maxRuntime, currentTime);
  processRuntime->avgRuntime += currentTime;
  processRuntime->processes++;
}

/**************************************************************************************
 * Return the position of the process in the given priority queue.
 */
int checkQueuePosition (PriorityQueue *queue, int *targetPid) {
  int pos = -1;
  for (int x = 0; x < queue->size; x++) {
    if (queue->process[x].pid == *targetPid) {
      pos = x;
      return pos;
    }
  }  
  return pos;
}

/**************************************************************************************
 * Push a new process into the priority queue.
 */
void push (PriorityQueue *queue, int pid, int priority, enum Status status, double queueTime) {
  if (queue->size + 1 >= queue->maxSize) {
    queue->maxSize = queue->maxSize ? queue->maxSize * 2 : 4;
    queue->process = (ProcessNode *)realloc(queue->process, queue->maxSize * sizeof (ProcessNode));
  }
  
  int i = queue->size + 1;
  int j = i / 2;
  
  while (i > 1 && queue->process[j].priority > priority) {
    queue->process[i] = queue->process[j];
    i = j;
    j = j / 2;
  }

  queue->process[i].timeInQueue = queueTime;
  queue->process[i].priority = priority;
  queue->process[i].pid = pid;
  queue->size++;
}

/**************************************************************************************
 * Pop the process from the queue.
 */
int pop (PriorityQueue *queue, int pid) {
  int i;
  int j;
  int k;
  int currPid;
  int currPriority;
  double currTimeInQueue;
  enum Status currStatus;
  
  if (!queue->size) {
    return -1;
  }

  currPid = queue->process[1].pid;
  currStatus = queue->process[1].status;
  currPriority = queue->process[1].priority;
  currTimeInQueue = queue->process[1].timeInQueue;
  
  queue->process[1] = queue->process[queue->size];
  queue->size--;
  i = 1;
  
  while (1) {
    k = i;
    j = 2 * i;
    
    if (j <= queue->size && queue->process[j].priority < queue->process[k].priority) {
      k = j;
    }
    
    if (j + 1 <= queue->size && queue->process[j + 1].priority < queue->process[k].priority) {
      k = j + 1;
    }
    
    if (queue->process[i].pid == pid) {
      break;
    }
    
    if (k == i) {
      break;
    }
    
    queue->process[i] = queue->process[k];
    i = k;
  }
  
  queue->process[i] = queue->process[queue->size + 1];
  
  return currPid;
}

/**************************************************************************************
 * A new process has been created.
 */
void NewProcess(int pid) {
  printf("New process %d created\n", pid);
  Ready(pid, 0);  
}

/**************************************************************************************
 * Update the runtime of the process and remove the process from the ready queue.
 */
void Dispatch(int *pid) {
  int pQueuePos;
  int wQueuePos;
  int pCurrPid;
  int wCurrPid;

  pQueuePos = checkQueuePosition(pQueue, pid);
  wQueuePos = checkQueuePosition(wQueue, pid);

  if (wQueuePos != -1 && wQueue->process[wQueuePos].timeInQueue > 0.00) {
    configRuntime(waitRuntime, wQueue->process[wQueuePos]);
    wCurrPid = pop(wQueue, *pid);
  }

  if (pQueuePos != -1) {
    configRuntime(readyRuntime, pQueue->process[pQueuePos]);
  }
  
  pCurrPid = pop(pQueue, *pid);
  
  *pid = pCurrPid;
  printf("Process %d dispatched\n", *pid);
}

/**************************************************************************************
 * If the process is in the waiting queue, add it to the ready queue after updating
 * the waiting runtime. If the process is not in any queue, add it to the ready queue
 * with priority 8, otherwie update the priority and add it to the ready queue.
 */
void Ready(int pid, int CPUtimeUsed) {
  int pQueuePos = -1;
  int wQueuePos = -1;
  double wTime;
  double pTime;
  int currPid = -1;
  int currPriority = -1;
  int wPid;
  int wPriority;
  enum Status currStatus;
  
  overheadRuntime->CPURuntime += CPUtimeUsed;
  wQueuePos = checkQueuePosition(wQueue, &pid);

  if (wQueuePos != -1) {
    wTime = getCurrentTime();
    wPriority = wQueue->process[wQueuePos].priority;
    wPriority++;
    wPid = pop(wQueue, pid);
    push(pQueue, wPid, wPriority, READY, wTime);
  }

  pQueuePos = checkQueuePosition(pQueue, &pid);
  pTime = getCurrentTime();
  
  if (pQueuePos == -1) {
    currPriority = 8;
    push(pQueue, pid, 8, READY, pTime);
    pQueuePos = checkQueuePosition(pQueue, &pid);

    if (pQueuePos != -1) {
      pQueue->process[pQueuePos].timeInQueue = getCurrentTime();
    }
  } else {
    currStatus = pQueue->process[pQueuePos].status;
    currPriority = pQueue->process[pQueuePos].priority;
    currPid = pop(pQueue, pid);
    
    if (currPriority != 0 && CPUtimeUsed == 100) {
      currPriority--;
    }
    if (currPriority != 14 && currStatus == WAIT) {
      currPriority++;
    }

    push(pQueue, currPid, currPriority, READY, pTime);
  }

  pQueuePos = checkQueuePosition(pQueue, &pid);
  if (pQueuePos != -1) {
    configRuntime(readyRuntime, pQueue->process[pQueuePos]);
  }

  printf("Process %d added to READY priority queue at %d msec\n", pid, CPUtimeUsed);
}

/**************************************************************************************
 * Process is added to the waiting queue if it's not in there. Otherwise, update the
 * waiting time and decrement the priority by 1.
 */
void Waiting(int pid) {
  int queuePos;

  queuePos = checkQueuePosition(wQueue, &pid);

  if (queuePos == -1) {
    push(wQueue, pid, 8, WAIT, 0.00);
  } else {
    wQueue->process[queuePos].timeInQueue = getCurrentTime();
    wQueue->process[queuePos].priority--;
    wQueue->process[queuePos].status = WAIT;
  }
  
  printf("Process %d waiting\n", pid);
}

/**************************************************************************************
 * Terminate the process.
 */
void Terminate(int pid) {
  printf("Process %d terminated\n", pid);
}

/**************************************************************************************
 * Main function, run the simulation and output the max, min and average runtime for
 * the process in ready and waiting queue along with the overhead of the whole execution.
 */
int main() {
  initializeStructs();

  overheadRuntime->minRuntime = getCurrentTime();
  Simulate(1000, 100);
  overheadRuntime->maxRuntime = getCurrentTime();

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
