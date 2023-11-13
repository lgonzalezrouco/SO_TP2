#include <shellPrograms.h>
#include <syscalls.h>
#include <uStdio.h>

#define MAX_SIZE 1024

int wc(int argc, char **argv) {
	if (argc != 1) {
		printErr("wc: Numero invalido de argumentos.\n");
		return -1;
	}

	char buffer[MAX_SIZE];

	int count;
	int counter = 0;

	while ((count = read(STDIN, buffer, MAX_SIZE - 1))) {
		printf("%s", buffer);
		buffer[count] = 0;
		for (int i = 0; i < count; i++)
			if (buffer[i] == '\n')
				counter++;
	}

	printf("El texto ingresado tiene %d lineas.\n", counter);
	return 0;
}