#ifndef _MEMORY_MANAGER_H_
#define _MEMORY_MANAGER_H_

#include <stddef.h>
#include <stdint.h>

typedef struct memoryInfo {
  int totalMemory;
  int usedMemory;
  int freeMemory;
} memoryInfo;

void initializeMemoryManager();

void *malloc(size_t memAmount);

void free(void *ptr);

memoryInfo *getMemoryInfo();

#endif