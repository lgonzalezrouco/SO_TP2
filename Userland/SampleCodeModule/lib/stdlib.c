// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define toLower(n)  ((n) >= 'A' && (n) <= 'Z' ? (n) - ('A' - 'a') : (n))
#define isNumber(n) ((n) >= '0' && (n) <= '9')
#define isHex(n)    ((n) >= 'a' && (n) <= 'f')

int createProcess(int16_t parentPid, ProcessCode code, char **args, char *name, uint8_t priority) {
	int fds[] = {STDIN, STDOUT, STDERR};
	return createProcessFds(parentPid, code, args, name, priority, fds);
}

static unsigned int log(uint64_t n, int base) {
	unsigned int count = 1;
	while (n /= base)
		count++;
	return count;
}

int itoa(uint64_t n, char *buffer, int base) {
	if (n == 0) {
		buffer[0] = '0';
		buffer[1] = '\0';
		return 1;
	}

	unsigned int len = 0;
	int i = 0;
	if (n < 0 && base == 10) {
		n = -n;
		buffer[i] = '-';
		len++;
		i++;
	}

	len += log(n, base);
	while (n != 0) {
		int r = n % base;
		buffer[len - i++ - 1] = (r > 9) ? (r - 10) + 'A' : r + '0';
		n /= base;
	}
	buffer[i] = '\0';
	return len;
}

int atoi(char *s) {
	int num = 0;
	while (isNumber(*s))
		num = num * 10 + *(s++) - '0';
	return num;
}

int strtoi(char *s, char **end) {
	int num = 0;
	while (isNumber(*s))
		num = num * 10 + *(s++) - '0';
	*end = s;
	return num;
}

int strtoh(char *s, char **end) {
	int num = 0;
	int aux;
	while (isNumber(*s) || isHex(*s)) {
		aux = toLower(*s);
		num = num * 16 + isNumber(aux) * (aux - '0') + isHex(aux) * (aux - 'a' + 10);
		s++;
	}
	*end = s;
	return num;
}

void *stringToPtr(char *buffer) {
	uint64_t res = 0;
	for (int i = 0; buffer[i] != '\0'; i++)
		res = res * 10 + buffer[i] - '0';
	return (void *) res;
}

int intToBase(unsigned long long num, int base, char *buffer) {
	char stack[11];
	int c = 0;
	int i = 0;
	if (num == 0)
		stack[i++] = '0';
	while (num != 0) {
		int remainder = num % base;
		stack[i] = remainder >= 10 ? remainder + 'A' - 10 : remainder + '0';
		num = num / base;
		i++;
	}
	c = i;
	i--;
	while (i >= 0) {
		*buffer = stack[i];
		buffer++;
		i--;
	}
	*buffer = 0;
	return c;
}

int intToString(unsigned long long num, char *buffer) {
	return intToBase(num, 10, buffer);
}