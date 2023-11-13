#ifndef _MEMORY_MANAGER_H_
#define _MEMORY_MANAGER_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define HEAP_SIZE         ((size_t) (128 * 1024 * 1024))
#define HEAP_ADDRESS_LIST (0x600000)
#define HEAP_ADDRESS      (HEAP_ADDRESS_LIST + 0x100000)

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