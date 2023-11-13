// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <shellPrograms.h>
#include <syscalls.h>
#include <types.h>
#include <uStdio.h>
#include <uStdlib.h>

int nice(int argc, char **argv) {
	if (argc != 3) {
		printErr("nice: Numero invalido de argumentos.\n");
		return -1;
	}
	char *pid = argv[1];
	char *priority = argv[2];

	int result = setPriority((int16_t) atoi(pid), (uint8_t) atoi(priority));
	if (result == NOT_FOUND) {
		printErr("Es inexistente el proceso con pid ");
		printf("%s\n", pid);
		return result;
	} else if (result == INVALID_PROCESS) {
		printErr("No se puede cambiar la prioridad al proceso con pid ");
		printf("%s\n", pid);
		return result;
	} else if (result == INVALID_PRIORITY) {
		printErr("La prioridad ingresada es invalida, debe ser entre 0 y 6\n");
		return result;
	} else if (result == SAME_PRIORITY) {
		printErr("La prioridad ingresada es la misma que la actual\n");
		return result;
	} else
		printf("Se cambio la prioridad del proceso %s a %s\n", pid, priority);

	return 0;
}