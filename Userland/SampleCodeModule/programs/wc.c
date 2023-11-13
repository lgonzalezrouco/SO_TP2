// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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

	int *fds = (int *) malloc(sizeof(int) * STD_FDS);
	fds = getFds();

	while ((c = getchar()) != EOF) {
		if (c != 0) {
			if (c != '\b') {
				if (c == '\n' && cIdx > 0) {
					if (fds[STDIN] < STD_FDS)
						putchar(c);
					counter++;
					cIdx = 0;
				} else if (c != '\n') {
					if (fds[STDIN] < STD_FDS)
						putchar(c);
					cIdx++;
				}
			} else if (cIdx > 0) {
				cIdx--;
				if (fds[STDIN] < STD_FDS)
					putchar(c);
			}
		}
	}
	printf("El texto ingresado tiene %d %s.\n", counter, counter == 1 ? "linea" : "lineas");
	putchar('\n');

	free(fds);
	return 0;
}