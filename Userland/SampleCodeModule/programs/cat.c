#include <shellPrograms.h>
#include <syscalls.h>
#include <uStdio.h>

#define MAX_SIZE 1024

int cat(int argc, char **argv) {
	if (argc != 1) {
		printErr("cat: Numero invalido de argumentos.\n");
		return -1;
	}

	char buffer[MAX_SIZE];

	int count;
	while ((count = read(0, buffer, MAX_SIZE - 1))) {
		buffer[count] = 0;
		puts(buffer);
	}
	
	return 0;
}