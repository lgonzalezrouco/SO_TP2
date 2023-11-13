#include <shellPrograms.h>
#include <syscalls.h>
#include <uStdio.h>

#define MAX_SIZE 1024

#define isVowel(c) \
	(c == 'a' || c == 'A' || \
	c == 'e' || c == 'E' || \
	c == 'i' || c == 'I' || \
	c == 'o' || c == 'O' || \
	c == 'u' || c == 'U')

int filter(int argc, char **argv) {
	if (argc != 1) {
		printErr("filter: Numero invalido de argumentos.\n");
		return -1;
	}

	char buffer[MAX_SIZE];

    int count;
    int index = 0;

    while ((count = read(0, buffer, MAX_SIZE - 1))) {
        buffer[count] = 0;
        for (int i = 0; i < count ; i++){
            if (!isVowel(buffer[i])) {
                buffer[index] = buffer[i];
                index++;
            }
        }
        buffer[index] = 0;
        index = 0;
        puts(buffer);
    }
    return 0;
}