#include <shellPrograms.h>
#include <syscalls.h>
#include <uStdio.h>

int wc(int argc, char **argv) {
	if (argc != 1) {
		printErr("wc: Numero invalido de argumentos.\n");
		return -1;
	}

	char c;
	int cIdx = 0;
	int counter = 0;

	while ((c = getchar()) != EOF) {
		if (c != 0) {
			if (c != '\b') {
				if (c == '\n' && cIdx > 0) {
					putchar(c);
					counter++;
					cIdx = 0;
				} else if (c != '\n') {
					putchar(c);
					cIdx++;
				}
			} else if (cIdx > 0) {
				cIdx--;
				putchar(c);
			}
		}
	}
	printf("El texto ingresado tiene %d lineas.\n", counter);
	return 0;
}