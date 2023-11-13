#include <shellPrograms.h>
#include <syscalls.h>
#include <uStdio.h>

#define MAX_SIZE 1024

#define isVowel(c)                                                                                               \
	(c == 'a' || c == 'A' || c == 'e' || c == 'E' || c == 'i' || c == 'I' || c == 'o' || c == 'O' || c == 'u' || \
	 c == 'U')

int filter(int argc, char **argv) {
	if (argc != 1) {
		printErr("filter: Numero invalido de argumentos.\n");
		return -1;
	}

	char c;
	char buffer[MAX_SIZE];
	int bIdx = 0;
	int lIdx = 0;

	int *fds = (int *) malloc(sizeof(int) * STD_FDS);
	fds = getFds();

	while ((c = getchar()) != EOF && bIdx < MAX_SIZE - 1) {
		if (c != 0) {
			if (c != '\b') {
				if (c == '\n' && lIdx > 0) {
					if (fds[STDIN] < STD_FDS)
						putchar(c);
					lIdx = 0;
					buffer[bIdx++] = c;
				} else if (c != '\n') {
					if (fds[STDIN] < STD_FDS)
						putchar(c);
					lIdx++;
					if (isVowel(c))
						buffer[bIdx++] = c;
				}
			} else if (lIdx > 0) {
				lIdx--;
				if (fds[STDIN] < STD_FDS)
					putchar(c);
			}
		}
	}
	buffer[bIdx] = 0;
	printf("\nTEXTO FILTRADO:\n\n%s\n", buffer);
	free(fds);
	return 0;
}