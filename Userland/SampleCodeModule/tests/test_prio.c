#include "syscalls.h"
#include "test_util.h"
#include "types.h"

#include <shellPrograms.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define MINOR_WAIT "500000000"  // TODO: Change this value to prevent a process from flooding the screen
#define WAIT \
	1000000000  // TODO: Change this value to make the wait long enough to see theese processes beeing run at least
	            // twice

#define TOTAL_PROCESSES 3
#define LOWEST          1  // TODO: Change as required
#define MEDIUM          3  // TODO: Change as required
#define HIGHEST         5  // TODO: Change as required

int64_t prio[TOTAL_PROCESSES] = {LOWEST, MEDIUM, HIGHEST};

int test_prio(int argc, char **argv) {
	if (argc != 1) {
		printErr("test_prio: Invalid number of arguments\n");
		return -1;
	}

	int64_t pids[TOTAL_PROCESSES];
	char *args[] = {"endless_loop_print", MINOR_WAIT, "0", NULL};
	uint64_t i;

	for (i = 0; i < TOTAL_PROCESSES; i++)
		pids[i] = createProcess(1, (ProcessCode) &endless_loop, args, "endless_loop_print", 0);

	busyWait(WAIT);
	printf("\nCHANGING PRIORITIES...\n");

	for (i = 0; i < TOTAL_PROCESSES; i++)
		setPriority(pids[i], prio[i]);

	busyWait(WAIT);
	printf("\nBLOCKING...\n");

	for (i = 0; i < TOTAL_PROCESSES; i++)
		toggleBlockProcess(pids[i]);

	printf("CHANGING PRIORITIES WHILE BLOCKED...\n");

	for (i = 0; i < TOTAL_PROCESSES; i++)
		setPriority(pids[i], MEDIUM);

	printf("UNBLOCKING...\n");

	for (i = 0; i < TOTAL_PROCESSES; i++)
		toggleBlockProcess(pids[i]);

	busyWait(WAIT);
	printf("\nKILLING...\n");

	for (i = 0; i < TOTAL_PROCESSES; i++)
		killProcess(pids[i]);

	return 0;
}