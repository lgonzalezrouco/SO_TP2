#include <stdint.h>
#include <syscalls.h>
#include <test_util.h>
#include <types.h>
#include <uStdio.h>
#include <uStdlib.h>

int64_t global;  // shared memory

void slowInc(int64_t *p, int64_t inc) {
	uint64_t aux = *p;
	yield();  // This makes the race condition highly probable
	aux += inc;
	*p = aux;
}

uint64_t my_process_inc(int argc, char *argv[]) {
	uint64_t n;
	int8_t inc;
	int8_t use_sem;

	if (argc != 4)
		return -1;

	if ((n = satoi(argv[1])) <= 0) {
		printf("test_sync: ERROR in first parameter, should be greater than 0, n = %d.\n", n);
		return -1;
	}
	if ((inc = satoi(argv[2])) == 0) {
		printf("test_sync: ERROR in second parameter, should be different from 0, inc = %d.\n", inc);
		return -1;
	}
	if ((use_sem = satoi(argv[3])) < 0) {
		printf("test_sync: ERROR in third parameter, should be greater or equal to 0, use_sem = %d.\n", use_sem);
		return -1;
	}

	if (use_sem) {
		if (semOpen(SEM_ID) == -1) {
			printf("test_sync: ERROR opening semaphore\n");
			return -1;
		}
	}

	uint64_t i;
	for (i = 0; i < n; i++) {
		if (use_sem)
			semWait(SEM_ID);
		slowInc(&global, inc);
		if (use_sem)
			semPost(SEM_ID);
	}

	return 0;
}

uint64_t test_sync(int argc, char **argv) {
	if (argc != 3) {
		printErr("test_sync: Invalid number of arguments\n");
		return -1;
	}

	char *n = argv[1];
	char *use_sem = argv[2];

	if (satoi(n) <= 0) {
		printf("test_sync: ERROR in first parameter, should be greater than 0.\n");
		return -1;
	}

	if (satoi(use_sem) < 0 || satoi(use_sem) > 1) {
		printf("test_sync: ERROR in second parameter, should be 0 or 1.\n");
		return -1;
	}

	if (satoi(use_sem)) {
		if (semInit(SEM_ID, 1) == -1) {
			printf("test_sync: ERROR creating semaphore\n");
			return -1;
		}
	}

	uint64_t pids[2 * TOTAL_PAIR_PROCESSES];

	char *argvDec[] = {"my_process_dec", n, "-1", use_sem, NULL};
	char *argvInc[] = {"my_process_inc", n, "1", use_sem, NULL};

	global = 0;

	uint64_t i;
	for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
		pids[i] = createProcess((ProcessCode) &my_process_inc, argvDec, "my_process_inc", 1);
		pids[i + TOTAL_PAIR_PROCESSES] = createProcess((ProcessCode) &my_process_inc, argvInc, "my_process_inc", 1);
	}

	for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
		waitpid(pids[i]);
		waitpid(pids[i + TOTAL_PAIR_PROCESSES]);
	}

	printf("Final value: %d\n", global);

	if (satoi(use_sem))
		semClose(SEM_ID);

	return 0;
}