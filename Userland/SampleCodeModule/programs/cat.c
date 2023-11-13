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

	int *fds = (int *) malloc(sizeof(int) * STD_FDS);
	fds = getFds();

	while ((c = getchar()) != EOF && bIdx < MAX_CHARS - 1) {
		if (c != 0) {
			if (c != '\b') {
				buffer[bIdx++] = c;
				if (fds[STDIN] < STD_FDS)
					putchar(c);
				if (c == '\n') {
					buffer[bIdx] = 0;
					printf("%s", buffer);
					bIdx = 0;
				}
			} else if (bIdx > 0) {
				bIdx--;
				if (fds[STDIN] < STD_FDS)
					putchar(c);
			}
		}
	}
	putchar('\n');
	free(fds);
	return 0;
}