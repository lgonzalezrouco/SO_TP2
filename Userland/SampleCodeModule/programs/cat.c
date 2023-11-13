#include <shellPrograms.h>
#include <syscalls.h>
#include <uStdio.h>

int cat(int argc, char **argv) {
	if (argc != 1) {
		printErr("cat: Numero invalido de argumentos.\n");
		return -1;
	}

	char c;
	char buffer[MAX_CHARS];
	uint64_t bIdx = 0;
	while ((c = getchar()) != EOF && bIdx < MAX_CHARS - 1) {
		if (c != 0) {
			if (c != '\b') {
				buffer[bIdx++] = c;
				putchar(c);
				if (c == '\n') {
					buffer[bIdx] = 0;
					printf("%s", buffer);
					bIdx = 0;
				}
			} else if (bIdx > 0) {
				bIdx--;
				putchar(c);
			}
		}
	}
	putchar('\n');

	return 0;
}