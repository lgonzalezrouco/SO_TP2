// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <memoryManager.h>
#include <stddef.h>
#include <stdint.h>
#define MAX_SHARED_MEMORY 512

void initializeSharedMemory();
void *openSharedMemory(int id, int size);