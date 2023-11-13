#include <sharedMemory.h>

void *sharedMemoryNames[MAX_SHARED_MEMORY];

void initializeSharedMemory() {
	for (int i = 0; i < MAX_SHARED_MEMORY; i++)
		sharedMemoryNames[i] = NULL;
}

void *openSharedMemory(int id, int size) {
	if (sharedMemoryNames[id] != NULL)
		return sharedMemoryNames[id];

	sharedMemoryNames[id] = malloc(size);
	if (sharedMemoryNames[id] == NULL)
		return NULL;

	return sharedMemoryNames[id];
}