#include <shellPrograms.h>
#include <syscalls.h>
#include <test_util.h>
#include <types.h>
#include <uStdio.h>
#include <uStdlib.h>

#define DEFAULT_SECONDS 1

int loop(int argc, char **argv) {
	if (argc != 2) {
		printErr("loop: Numero invalido de argumentos.\n");
		return -1;
	}

	int seconds = satoi(argv[1]);

	if (seconds <= 0) {
		printErr("loop: Los segundos no pueden ser negativos ni 0.\n");
		return -1;
	}

	int16_t pid = getPid();
	while (1) {
		sleep(seconds);
		printf("Hola! Soy el proceso %d e imprimo este mensaje cada %d %s.\n",
		       pid,
		       seconds,
		       seconds == 1 ? "segundo" : "segundos");
		yield();
	}
	return 0;
}