#include <shellPrograms.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syscalls.h>
#include <types.h>

int kill(int argc, char **argv) {
	if (argc != 2) {
		printErr("kill: Invalid number of arguments\n");
		return -1;
	}
	char pid[10];
	strcpy(pid, argv[1]);

	int result = killProcess((int16_t) atoi(pid));
	if (result == NOT_FOUND) {
		printErr("Es inexistente el proceso con pid ");
		printf("%s\n", pid);
		return result;
	} else if (result == INVALID_PROCESS) {
		printErr("No se puede matar al proceso con pid ");
		printf("%s\n", pid);
		return result;
	} else
		printf("Se mato al proceso con pid %s\n", pid);

	return 0;
}