/*
 * lruPageReplacer.c
 *
 * Raymond Weiming Luo	
 * CSCI 460 - Operating Systems
 * Assignment 3: Page Replacement Algorithm
 *
 * Implementation of the Least Recently Used, Enhanced Second Chance 
 * algorithm. THe page being replaced follows the critera based on the
 * reference and modified (dirty) bits.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "MemSim.h"
#include "Assg3.h"

#define MAXPROC	64
#define TABLESIZE 97
#define PAGESIZE 4096

typedef struct procstruct {
  int pid;
  int page;
  int valid;
  int modify;
  int reference;
} proc;

int readPageCount;
int writePageCount;
int procCount = 0;
int prevPageReplace = 0;
int pidTable[MAXPROC];
proc* table[TABLESIZE];

/********************************************************************************
 *  Find the PID if it's in the table of listed PIDs.
 */
int FindPid (int pid) {
  for (int i = 0; i < MAXPROC; i++) {
    if (pidTable[i] == pid) {
      return i;
    }
  }
  return -1;
}

/******************************************************************************** 
 * Find the page in the page table and increment the reference count.
 * If the page has not been written, modify the bit to corresponding
 * write bit.
 */
int FindPage (int pid, int page, int write) {
  for (int i = 0; i < TABLESIZE; i++) {
    if (table[i]->pid == pid && table[i]->page == page && table[i]->valid == 1) {
      table[i]->reference = 1;

      if (table[i]->modify == 0) {
        table[i]->modify = write;
      }
      return i;
    }
  }
  return -1;
}

/********************************************************************************
 * Add the PID to the PID table, max of 64 PIDs can be added at a time.
 */
int AddPid (int pid) {  
  for (int i = 0; i < MAXPROC; i++) {
    if (pidTable[i] == -1) {
      pidTable[i] = pid;
      procCount++;
      return i;
    }
  }

  fprintf(stderr, "Pid table does not have available space, can't add pid %d\n", pid);
  return -1;
}

/********************************************************************************
 * Invert the page table and put the chosen page to the front.
 */
int InvertTable (int endIndex) {
  int i;
  int tempPid = table[endIndex]->pid;
  int tempPage = table[endIndex]->page;
  int tempValid = table[endIndex]->valid;
  int tempModify = table[endIndex]->modify;
  int tempReference = table[endIndex]->reference;

  for (i = endIndex; i > 0; i--) {
    table[i]->pid = table[i-1]->pid;
    table[i]->page = table[i-1]->page;
    table[i]->valid = table[i-1]->valid;
    table[i]->modify = table[i-1]->modify;
    table[i]->reference = table[i-1]->reference;
  }

  table[0]->pid = tempPid;
  table[0]->page = tempPage;
  table[0]->valid = tempValid;
  table[0]->modify = tempModify;
  table[0]->reference = tempReference;
}

/********************************************************************************
 * Add the page to the front of the inverted page table.
 */
int AddPage (int pid, int page, int write) {
  for (int i = 0; i < TABLESIZE; i++) {
    if (table[i]->valid == 0) {
      InvertTable(i);

      table[0]->pid = pid;
      table[0]->page = page;
      table[0]->valid = 1;
      table[0]->modify = write;
      table[0]->reference = 1;  
      return i;
    }
  }
  return -1;
}

/********************************************************************************
 * Replace the page with the new page in the front of the page table.
 * Page replacement follows the criteria by replacing pages in order of : 
 * Reference : 0 | Modify : 0
 * Reference : 0 | Modify : 1
 * Reference : 1 | Modify : 0
 * Reference : 1 | Modify : 1
 */
int ReplacePage (int pid, int page, int write) {
  int i;
  int k;
  int refBit = 0;
  int modBit = 0;

  while (refBit < 2 && modBit < 2) {
    for (i = 0; i < TABLESIZE; i++) {
      if (table[i]->valid == 1 && table[i]->reference == refBit && table[i]->modify == modBit) {
        if (table[i]->modify) {
          writePageCount++;
	} else {
          readPageCount++;
	}

        InvertTable(i);

        table[0]->pid = pid;
        table[0]->page = page;
        table[0]->valid = 1;
        table[0]->modify = write;
        table[0]->reference = 0;

        for (k = 1; k < i; k++) {
          table[k]->reference = 0;
        }
        return i;
      }
    }

    if (refBit == modBit) {
      modBit++;
    } else {
      refBit++;
    }
  }
  return -1;
}

/********************************************************************************
 * Remove the PID from the PID table and the corresponding page in
 * the page table.
 */
void Remove (int pid) {
  for (int i = 0; i < TABLESIZE; i++) {
    if (table[i]->pid == pid) {
      table[i]->valid = 0;
    }

    if (i < MAXPROC && pidTable[i] == pid) {
      pidTable[i] = -1;
      procCount--;
    }
  }
}

/********************************************************************************
 * Return the page of the corresponding PID from the page table.
 * If PID is not listed in PID table, add it then create a new page.
 */
int Access (int pid, int address, int write) {
  int offset;
  int page;
  int newAddr;
  int tableIndex;
  
  page = address >> 12;
  offset = address & 0xFFF;

  if (FindPid(pid) != -1) {
    for (int i = 0; i < 3; i++) {
      if (i == 0) {
        tableIndex = FindPage(pid, page, write);
      } else if (i == 1) {
        tableIndex = AddPage(pid, page, write);
      } else if (i == 2) {
        tableIndex = ReplacePage(pid, page, write);
      }
      
      if (tableIndex > -1) {
        newAddr = ((tableIndex << 12) | offset);
        printf("pid %d wants %s access to address %d on page %d\n", 
               pid, (write) ? "write" : "read", address, (address/PAGESIZE));
        return newAddr;
      }
    }
  } else {
    if (procCount >= MAXPROC) {
      printf("pid %d refused\n", pid);
      return 0;
    }

    if (AddPid(pid) != -1) {
      for (int i = 0; i < 2; i++) {
        if (i == 0) {
          tableIndex = AddPage(pid, page, write);
	} else if (i == 1) {
          tableIndex = ReplacePage(pid, page, write);
	}

        if (tableIndex > -1) {
          return ((tableIndex << 12) | offset);
        }
      }
    }
    return 0;
  }
}

/********************************************************************************
 * Terminate the pid and remove it from the PID table and 
 * corresponding page table.
 */
void Terminate (int pid) {
  printf("pid %d terminated\n", pid);
  Remove(pid);
}

/********************************************************************************
 * Initiate the PID and page table. Start the simulation and output the 
 * page fault data.
 */
void main () {
  int i;
  proc *node = NULL;

  for (i = 0; i < MAXPROC; i++) {
    pidTable[i] = -1;
  }

  for (i = 0; i < TABLESIZE; i++) {
    node = (proc *) malloc(sizeof (proc));
    node->pid = 0;
    node->page = 0;
    node->valid = 0;
    node->modify = 0;
    node->reference = 0;
    table[i] = node;
  }
  
  printf("================== LRU simulation started ==================\n");
  Simulate(1000);
  printf("================= LRU simulation completed =================\n\n");

  printf("Reading page faults             : %d\n"
         "Writing and reading page faults : %d\n"
         "Total page faults               : %d\n"
         ,readPageCount, writePageCount, (readPageCount+writePageCount));
}
