#include <shellPrograms.h>
#include <stdint.h>
#include <syscalls.h>
#include <test_util.h>
#include <types.h>
#include <uStdio.h>
#include <uStdlib.h>

typedef struct P_rq {
	int32_t pid;
	enum processStatus state;
} p_rq;

int test_processes(int argc, char **argv) {
	if (argc != 2) {
		printErr("test_processes: Numero invalido de argumentos.\n");
		return -1;
	}

	printf("test_processes: Starting\n");
	uint8_t rq;
	uint8_t alive = 0;
	uint8_t action;
	if (argv[1] == NULL) {
		printErr("test_processes: No se ingreso la cantidad de procesos a crear\n");
		return -1;
	}
	uint64_t max_processes = atoi(argv[1]);
	char *argvAux[] = {0};

	p_rq p_rqs[max_processes];

	while (1) {
		// Create max_processes processes
		for (rq = 0; rq < max_processes; rq++) {
			// printf("Creating process\n");
			p_rqs[rq].pid = createProcess((ProcessCode) &endless_loop, argvAux, "endless_loop", 1);
			if (p_rqs[rq].pid == -1) {
				printf("test_processes: ERROR creating process\n");
				return -1;
			} else {
				// printf("Created process %d\n", p_rqs[rq].pid);
				p_rqs[rq].state = RUNNING;
				alive++;
			}
		}

		// Randomly kills, blocks or unblocks processes until every one has been
		// killed
		while (alive > 0) {
			for (rq = 0; rq < max_processes; rq++) {
				// printf("Checking process %d\n", p_rqs[rq].pid);
				action = GetUniform(100) % 2;

				switch (action) {
					case 0:
						if (p_rqs[rq].state == RUNNING || p_rqs[rq].state == BLOCKED) {
							printf("Killing process %d\n", p_rqs[rq].pid);
							int result = killProcess(p_rqs[rq].pid);
							if (result == NOT_FOUND) {
								printErr("Es inexistente el proceso con pid ");
								printf("%s\n", p_rqs[rq].pid);
							} else if (result == INVALID_PROCESS) {
								printErr("No se puede matar al proceso con pid ");
								printf("%s\n", p_rqs[rq].pid);
							}
							printf("Killed process %d\n", p_rqs[rq].pid);
							p_rqs[rq].state = KILLED;
							alive--;
						}
						break;

					case 1:
						if (p_rqs[rq].state == RUNNING) {
							printf("Blocking process %d\n", p_rqs[rq].pid);
							if (toggleBlockProcess(p_rqs[rq].pid) == -1) {
								printf("test_processes: ERROR blocking process\n");
								return -1;
							}
							printf("Blocked process %d\n", p_rqs[rq].pid);
							p_rqs[rq].state = BLOCKED;
						}
						break;
				}
			}

			// Randomly unblocks processes
			for (rq = 0; rq < max_processes; rq++) {
				if (p_rqs[rq].state == BLOCKED && GetUniform(100) % 2) {
					printf("Unblocking process %d\n", p_rqs[rq].pid);
					if (toggleBlockProcess(p_rqs[rq].pid) == -1) {
						printf("test_processes: ERROR unblocking process\n");
						return -1;
					}
					printf("Unblocked process %d\n", p_rqs[rq].pid);
					p_rqs[rq].state = RUNNING;
				}
			}
		}
	}
}