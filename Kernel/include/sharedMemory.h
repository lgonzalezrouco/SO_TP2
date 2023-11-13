#include <stddef.h>
#include <stdint.h>
#include <memoryManager.h>
#define MAX_SHARED_MEMORY 512

void initializeSharedMemory();
void *openSharedMemory(int id, int size);