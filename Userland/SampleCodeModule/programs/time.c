// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <shellPrograms.h>
#include <syscalls.h>
#include <uStdio.h>

int time(int argc, char **argv) {
	if (argc != 1) {
		printErr("time: Numero invalido de argumentos.\n");
		return -1;
	}

	uint32_t secs = getSeconds();
	uint32_t h = secs / 3600, m = secs % 3600 / 60, s = secs % 3600 % 60;
	printf("%2d:%2d:%2d\r\n", h, m, s);
	return 0;
}