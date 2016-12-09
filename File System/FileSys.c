/* FileSys.c
 *
 * Weiming Raymond Luo
 * CSCI 460 - Operating Systems
 * Assignment 4 : File System
 *
 * This program is a file system implmentation that stores the input Data
 * into the respective inode block. A list of free blocks is implemented
 * to find the available block spaces that was malloc'd for new Data to 
 * be stored into the inode.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include "Driver.h"
#include "FileSysAPI.h"

#define BYTES_PER_SECTOR 64

typedef struct freeblock {
  int size;
  int index;
  struct freeblock* next;
} freeblock;

typedef struct file {
  int size;
  char name[50];
  struct inode* inode;
  struct file* next;
} file;

typedef struct inode {
  int blocks[13];
  int singleInode;
  int doubleInode;
  int tripleInode;
} inode;

typedef struct single_inode {
  int blocks[16];
} single_inode;

typedef struct double_inode {
  struct single_inode* blocks[16];
} double_inode;

typedef struct triple_inode {
  struct double_inode* blocks[16];
} triple_inode;

int FS_STATUS = 0;
struct file* FileSystem = NULL;
struct freeblock* FreeBlocks = NULL;

/************************************************************************ 
 * Find the min size of the data block for inodes. */
int min (int a, int b) {
  return (((a)<(b))?(a):(b));
}

/************************************************************************
 * Find the next free block in the FreeBlockList and assign the size
 * needed to store the incoming data.
 */
int FindFreeBlock (int size) {
  int blockIndex = -1;
  struct freeblock* currentBlock = FreeBlocks;
  
  if (currentBlock == NULL)
    return 0;

  size = (size+BYTES_PER_SECTOR-1)/BYTES_PER_SECTOR;
  
  while (currentBlock != NULL){
    if (currentBlock->size >= size) {
      blockIndex = currentBlock->index;
      currentBlock->size -= size;
      currentBlock->index += size;
      
      return blockIndex;
    }
    currentBlock = currentBlock->next;
  }
  return blockIndex;
}

/************************************************************************
 * Find the next available inode that can store the data block given the 
 * Size. Return the status of which inode type is used (single, double, 
 * or triple) and set the freeBlock to the index of the free block.
 */
int FindFreeInode (int Size, int *freeBlock) {
  int inodeSize;
  
  Size = (Size+BYTES_PER_SECTOR-1)/BYTES_PER_SECTOR;

  if (Size > 4109)
    return -1;

  if (Size > 13) {
    inodeSize = sizeof(single_inode);
    *freeBlock = FindFreeBlock(inodeSize);
    return 1;
  }

  if (Size > 29) {
    inodeSize = sizeof(double_inode);
    *freeBlock = FindFreeBlock(inodeSize);
    return 2;
  }

  if (Size > 269) {
    inodeSize = sizeof(triple_inode);
    *freeBlock = FindFreeBlock(inodeSize);
    return 3;
  }

  return 0;
}

/************************************************************************
 * Format the new file system, free the current file system of free block
 * list is there is any. Set the FS_STATUS to 1 to ensure a file system is
 * created.
 */
int CSCI460_Format(){
  if (!DevFormat())
    return 0;

  if (FS_STATUS) {
    if (FileSystem != NULL)
      free(FileSystem);
  
    if (FreeBlocks != NULL)
      free(FreeBlocks);
  } else {
    FreeBlocks = (struct freeblock *) malloc(sizeof(struct freeblock));

    FreeBlocks->size = INT_MAX;
    FreeBlocks->index = 0;
    FreeBlocks->next = NULL;

    FS_STATUS = 1;
  }

  return 1;
}

/************************************************************************
 * Write the input Data into the repsected file given the FileName with
 * the relative Size. The written Data will be determined by the input 
 * Size to determine where it will be stored in the inode file system.
 */
int CSCI460_Write (char *FileName, int Size, char *Data) {
  int dataSize;
  int freeIndex;
  int freeBlock;
  int inodeSrc;
  int inodeDest;
  int inodeSector;
  int adjustedSize;
  int inodeStatus = 0;
  int writeStatus = 0;
  char buffer[BYTES_PER_SECTOR+1];
  struct file *newFile;
  struct file *currentFile = FileSystem;
  struct inode *newInode;
  struct inode *currentInode;
  struct single_inode *singleInode;
  struct double_inode *doubleInode;
  struct triple_inode *tripleInode;

  if (!FS_STATUS)
    return writeStatus;

  adjustedSize = (Size+BYTES_PER_SECTOR-1)/BYTES_PER_SECTOR;
  freeIndex = FindFreeBlock(Size);
  
  if (freeIndex == -1)
    return writeStatus;
  
  newFile = (struct file *) malloc(sizeof(struct file));
  currentInode = (struct inode *) malloc(sizeof(struct inode));
  
  strcpy(newFile->name, FileName);
  newFile->size = Size;
  newFile->inode = currentInode;
  newFile->next = NULL;
  newInode = newFile->inode;

  if (FileSystem == NULL){
    FileSystem = newFile;
  } else{
    while (currentFile->next != NULL){
      currentFile = currentFile->next;
    }
    currentFile->next = newFile;
  }
  
  inodeStatus = FindFreeInode(Size, &freeBlock);
  dataSize = min(adjustedSize, 13);

  if (inodeStatus == -1) {
    fprintf(stderr, "error : CSCI460_Write() failed as file size is larger than file system\n");
  }
  else if (inodeStatus == 0) {
    for (int i = 0; i < adjustedSize; i++) {
      strncpy(buffer, Data+(i * BYTES_PER_SECTOR), BYTES_PER_SECTOR);
      if (!DevWrite(freeIndex+i, buffer)){
        fprintf(stderr, "error : CSCI460_Write() failed when writing to file using driver\n");
        return 0;
      }
      newInode->blocks[i] = freeIndex+i;
    }
  }
  else if (inodeStatus == 1) {
    dataSize = min(adjustedSize, 29);
    singleInode = (struct single_inode *) malloc(sizeof(struct single_inode));
    
    for (int i = 13; i < adjustedSize; i++){
      strncpy(buffer, Data + (i*BYTES_PER_SECTOR), BYTES_PER_SECTOR);
      
      if (!DevWrite(freeIndex+i, buffer)){
        fprintf(stderr, "error : CSCI460_Write() failed when writing to file using driver\n");
        return 0;
      }
      singleInode->blocks[i-13] = freeIndex+i;
    }
    
    if (!DevWrite(freeBlock, (char *) singleInode)){
      fprintf(stderr, "error : CSCI460_Write() failed when writing to file using driver\n");
      return 0;
    }
    newInode->singleInode = freeBlock;
  }
  else if (inodeStatus == 2 || inodeStatus == 3) {
    if (inodeStatus == 2) {
      dataSize = min(adjustedSize, 269);
      inodeSector = ((adjustedSize-29)+16-1)/16;
      doubleInode = (struct double_inode *) malloc(sizeof(struct double_inode));
    } else {
      dataSize = min(adjustedSize, 4109);
      inodeSector = ((adjustedSize-269)+16-1)/16;
      tripleInode = (struct triple_inode *) malloc(sizeof(struct triple_inode));
    }

    for (int j = 0; j < inodeSector; j++) {
      inodeSrc = (j * 16) + 29;
      inodeDest = (j * 16) + 45;
      for (int i = inodeSrc; i < inodeDest; i++) {
        Data += (i * BYTES_PER_SECTOR);
        strncpy(buffer, Data, BYTES_PER_SECTOR);
        
        if (!DevWrite(freeIndex+i, buffer)) {
          fprintf(stderr, "error : CSCI460_Write() failed when writing to file using driver\n");
          return 0;
        }

        if (inodeStatus == 2)
          doubleInode->blocks[inodeSector]->blocks[i-29] = freeIndex+i;
        else
          tripleInode->blocks[inodeSector]->blocks[j-269]->blocks[i-29] = freeIndex+i;
      }
    }

    if (inodeStatus == 2) {
      if (!DevWrite(freeBlock, (char *) doubleInode)){
        fprintf(stderr, "error : CSCI460_Write() failed when writing to file using driver\n");
        return 0;
      }
      newInode->doubleInode = freeBlock;
    } else {
      if (!DevWrite(freeBlock, (char *) tripleInode)){
        fprintf(stderr, "error : CSCI460_Write() failed when writing to file using driver\n");
        return 0;
      }
      newInode->tripleInode = freeBlock;
    }
  }
  return 1;
}

/************************************************************************
 * Read the data that has been stored in the file with the repsected
 * FileName. 
 */
int CSCI460_Read (char *FileName, int MaxSize, char *Data) {
  int fileSize;
  int readStatus = 0;
  int inodeIndex = 0;
  char fileData[BYTES_PER_SECTOR + 1];
  struct file *currentFile = FileSystem;
  struct inode *currentInode;

  if (!FS_STATUS)
    return 0;
  
  while(currentFile != NULL){
    if (!(strcmp(currentFile->name, FileName))){
      currentInode = currentFile->inode;
      fileSize = ((currentFile->size)+BYTES_PER_SECTOR-1)/BYTES_PER_SECTOR;

      while (inodeIndex < fileSize){
        readStatus = DevRead(currentInode->blocks[inodeIndex], fileData);
        if(!readStatus){
          fprintf(stderr, "error : CSCI460_READ() reading from DevRead() failed\n");
          return readStatus;
        }
        
        Data += (inodeIndex * BYTES_PER_SECTOR);
        strcpy(Data, fileData);
        inodeIndex++;
      }
      return readStatus;
    }
    currentFile = currentFile->next;
  }
  return readStatus;
}

/************************************************************************
 * Delete the file that correlates with the given Filename, the file is
 * removed from the file system [FileSystem] and the memory that was
 * allocated to the file is added to the list of free blocks. [FreeBlocks]
 */
int CSCI460_Delete (char *Filename) {
  int deleteStatus = 0;
  struct file *currentFile;
  struct file *nextFile;
  struct freeblock *cleanBlock;

  if (!FS_STATUS)
    return deleteStatus;
  
  currentFile = FileSystem;

  if (currentFile == NULL)
    return deleteStatus;
  
  if (!(strcmp(currentFile->name, Filename))){
    FileSystem = FileSystem->next;
    deleteStatus = 1;
    return deleteStatus;
  }

  while (currentFile != NULL) {
    if (!(strcmp(currentFile->name, Filename))) {
      nextFile = currentFile;
      currentFile = currentFile->next;
      cleanBlock  = (struct freeblock *) malloc(sizeof(struct freeblock));
      cleanBlock->size = nextFile->size;
      cleanBlock->index = nextFile->inode->blocks[0];
      cleanBlock->next = FreeBlocks;
      FreeBlocks = cleanBlock;
      deleteStatus = 1;
      free(nextFile);

      return deleteStatus;
    }
    currentFile = currentFile->next;
  }
  
  return deleteStatus;
}
