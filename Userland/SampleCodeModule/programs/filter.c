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
	while ((c = getchar()) != EOF && bIdx < MAX_SIZE - 1) {
		if (c != 0) {
			if (c != '\b') {
				if (c == '\n' && lIdx > 0) {
					putchar(c);
					lIdx = 0;
					buffer[bIdx++] = c;
				} else if (c != '\n') {
					putchar(c);
					lIdx++;
					if (isVowel(c))
						buffer[bIdx++] = c;
				}
			} else if (lIdx > 0) {
				lIdx--;
				putchar(c);
			}
		}
	}
	buffer[bIdx] = 0;
	printf("\nTEXTO FILTRADO:\n\n%s\n", buffer);

	return 0;
}