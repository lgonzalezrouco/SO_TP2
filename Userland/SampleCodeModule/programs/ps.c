#include <shellPrograms.h>
#include <syscalls.h>
#include <types.h>
#include <uStdio.h>

int printProcesses(int argc, char **argv) {
	if (argc != 1) {
		printErr("ps: Numero invalido de argumentos.\n");
		return -1;
	}

	PCB **info = getProcessesInfo();
	int i = 0;

	// Encabezados de la tabla
	printf(
	    "PID\t\t\tPARENT PID\t\t\tPRIORITY\t\t\tSTATUS\t\t\tSTACK BASE\t\t\tSTACK POINTER\t\t\tFOREGROUND\t\t\tNAME\n");

	while (info[i] != NULL) {
		printf("%d\t\t\t", info[i]->pid);
		if (info[i]->pid < 100)
			printf("\t");
		if (info[i]->pid < 10)
			printf("\t");
		printf("%d\t\t\t\t\t\t\t\t\t\t", info[i]->parentPid);
		if (info[i]->parentPid < 100)
			printf("\t");
		if (info[i]->parentPid < 10)
			printf("\t");
		if (info[i]->priority == 6)
			printf("BLOCKED\t\t\t\t");
		else if (info[i]->priority == 0)
			printf("IDLE\t\t\t\t\t\t\t");
		else
			printf("%d\t\t\t\t\t\t\t\t\t\t", info[i]->priority);
		switch (info[i]->status) {
			case 0:
				printf("RUNNING\t\t");
				break;
			case 1:
				printf("BLOCKED\t\t");
				break;
			case 3:
				printf("READY\t\t\t\t");
				break;
			default:
				printf("UNKNOWN\t\t");
				break;
		}
		printf("0x%x\t\t\t\t\t", info[i]->stack->base);
		printf("0x%x\t\t\t\t\t\t\t\t", info[i]->stack->current);
		printf("%d\t\t\t\t\t\t\t\t\t\t\t\t", info[i]->isForeground ? 1 : 0);
		printf("%s", info[i]->name);
		putchar('\n');
		i++;
	}
	freeProcessesInfo(info);

	return 0;
}