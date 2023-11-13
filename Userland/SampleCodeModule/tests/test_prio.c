// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <shellPrograms.h>
#include <stdint.h>
#include <syscalls.h>
#include <test_util.h>
#include <types.h>
#include <uStdio.h>
#include <uStdlib.h>

#define MINOR_WAIT "25000000"
#define WAIT       50000000

#define TOTAL_PROCESSES 3
#define LOWEST          1
#define MEDIUM          3
#define HIGHEST         5

int64_t prio[TOTAL_PROCESSES] = {LOWEST, MEDIUM, HIGHEST};

int test_prio(int argc, char **argv) {
	if (argc != 1) {
		printErr("test_prio: Numero invalido de argumentos.\n");
		return -1;
	}

	printf("STARTING TEST...\n");

	int64_t pids[TOTAL_PROCESSES];
	char *args[] = {"endless_loop_print", MINOR_WAIT, "0", NULL};
	uint64_t i;

	for (i = 0; i < TOTAL_PROCESSES; i++)
		pids[i] = createProcess((ProcessCode) &endless_loop, args, "endless_loop_print", 1);

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

	printf("TEST FINISHED\n");
	return 0;
}