// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <shellPrograms.h>
#include <syscalls.h>
#include <types.h>
#include <uStdio.h>
#include <uStdlib.h>
#include <uString.h>

int kill(int argc, char **argv) {
	if (argc != 2) {
		printErr("kill: Numero invalido de argumentos.\n");
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