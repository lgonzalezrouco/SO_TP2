#include <stdio.h>

#include "syscalls.h"
#include "test_util.h"
#include "test_processes.h"

typedef struct P_rq
{
	int32_t pid;
	enum processStatus state;
} p_rq;

int64_t test_processes(char* cant)
{
	printf("test_processes: Starting\n");
	uint8_t rq;
	uint8_t alive = 0;
	uint8_t action;
	uint64_t max_processes = atoi(cant);
	char *argvAux[] = {0};

	p_rq p_rqs[max_processes];

	for (rq = 0; rq < max_processes; rq++)
	{
		// p_rqs[rq].pid = my_create_process("endless_loop", 0, argvAux);
		p_rqs[rq].pid = createProcess(1, (ProcessCode)&endless_loop, argvAux, "endless_loop", 0);
		if (p_rqs[rq].pid == -1)
		{
			printf("test_processes: ERROR creating process\n");
			return -1;
		}
		else
		{
			p_rqs[rq].state = RUNNING;
			alive++;
		}
	}

	for (rq = 0; rq < max_processes; rq++)
	{
		if (p_rqs[rq].state == RUNNING || p_rqs[rq].state == BLOCKED)
		{
			if (killProcess(p_rqs[rq].pid) == -1)
			{
				printf("test_processes: ERROR killing process\n");
				return -1;
			}
			p_rqs[rq].state = KILLED;
			alive--;
		}
	}
	// TODO DEJAR LO DE ABAJO EN VEZ DE LOS DOS FOR DE ARRIBA
	/*
	while (1)
	{
		// Create max_processes processes
		for (rq = 0; rq < max_processes; rq++)
		{
			// p_rqs[rq].pid = my_create_process("endless_loop", 0, argvAux);
			p_rqs[rq].pid = createProcess(1, (ProcessCode)&endless_loop, argvAux, "endless_loop", 0);
			if (p_rqs[rq].pid == -1)
			{
				printf("test_processes: ERROR creating process\n");
				return -1;
			}
			else
			{
				p_rqs[rq].state = RUNNING;
				alive++;
			}
		}

		// Randomly kills, blocks or unblocks processes until every one has been
		// killed
		while (alive > 0)
		{
			for (rq = 0; rq < max_processes; rq++)
			{
				action = GetUniform(100) % 2;

				switch (action)
				{
				case 0:
					if (p_rqs[rq].state == RUNNING || p_rqs[rq].state == BLOCKED)
					{
						if (my_kill(p_rqs[rq].pid) == -1)
						{
							printf("test_processes: ERROR killing process\n");
							return -1;
						}
						p_rqs[rq].state = KILLED;
						alive--;
					}
					break;

				case 1:
					if (p_rqs[rq].state == RUNNING)
					{
						if (my_block(p_rqs[rq].pid) == -1)
						{
							printf("test_processes: ERROR blocking process\n");
							return -1;
						}
						p_rqs[rq].state = BLOCKED;
					}
					break;
				}
			}

			// Randomly unblocks processes
			for (rq = 0; rq < max_processes; rq++)
				if (p_rqs[rq].state == BLOCKED && GetUniform(100) % 2)
				{
					if (my_unblock(p_rqs[rq].pid) == -1)
					{
						printf("test_processes: ERROR unblocking process\n");
						return -1;
					}
					p_rqs[rq].state = RUNNING;
				}
		}
	}
	*/
}