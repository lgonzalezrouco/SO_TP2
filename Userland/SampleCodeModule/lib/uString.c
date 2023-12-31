// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <uString.h>

int strcmp(const char *s1, const char *s2) {
	while (*s1 != 0 && *s1 == *s2) {
		s1++;
		s2++;
	}
	return *s1 - *s2;
}

int strlen(const char *s) {
	int i = 0;
	while (s[i] != 0)
		i++;
	return i;
}

int strcpy(char *dest, const char *origin) {
	return strcpychar(dest, origin, '\0');
}

int strcpychar(char *dest, const char *origin, char limit) {
	int idx = 0;
	while (origin[idx] != limit && origin[idx] != '\0') {
		dest[idx] = origin[idx];
		idx++;
	}
	dest[idx] = 0;
	return idx;
}