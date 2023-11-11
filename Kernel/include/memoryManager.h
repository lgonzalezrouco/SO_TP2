#ifndef _MEMORY_MANAGER_H_
#define _MEMORY_MANAGER_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct memoryInfo {
	int totalMemory;
	int usedMemory;
	int freeMemory;
} memoryInfo;

void initializeMemoryManager();

void *malloc(uint64_t size);

void free(void *ptr);

memoryInfo *getMemoryInfo();

#endif