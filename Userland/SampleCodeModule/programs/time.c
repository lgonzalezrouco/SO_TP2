#include <shellPrograms.h>
#include <stdio.h>
#include <syscalls.h>

int time(int argc, char **argv) {
	if (argc != 1) {
		printErr("time: Invalid number of arguments\n");
		return -1;
	}

	uint32_t secs = getSeconds();
	uint32_t h = secs / 3600, m = secs % 3600 / 60, s = secs % 3600 % 60;
	printf("%2d:%2d:%2d\r\n", h, m, s);
	return 0;
}