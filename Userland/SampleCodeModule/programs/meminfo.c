#include <shellPrograms.h>
#include <stdio.h>
#include <syscalls.h>
#include <types.h>

int printMemInfo(int argc, char **argv) {
	if (argc != 1) {
		printErr("meminfo: Invalid number of arguments\n");
		return -1;
	}

	memoryInfo *info = getMemoryInfo();

	printf("Total memory: %d B\n", info->totalMemory);
	printf("Used memory: %d B\n", info->usedMemory);
	printf("Free memory: %d B\n", info->freeMemory);

	return 0;
}