#ifndef _TYPES_H_U_
#define _TYPES_H_U_

#include <stddef.h>
#include <stdint.h>

typedef enum status {
	PIPE_ERROR = -8,
	INVALID_PID,
	SAME_STATUS,
	INVALID_STATUS,
	SAME_PRIORITY,
	INVALID_PRIORITY,
	NOT_FOUND,
	INVALID_PROCESS,
	SUCCESS
} status;

// 3 (stdin, stdout, stderr)
#define STDIN    0
#define STDOUT   1
#define STDERR   2
#define STD_FDS  3
#define EOF      -1
#define DEV_NULL -1

typedef enum processStatus { RUNNING, BLOCKED, KILLED, READY } processStatus;

typedef enum pipeModes { READ, WRITE } pipeModes;

typedef int (*ProcessCode)(int argc, char **args);

typedef struct memoryInfo {
	int totalMemory;
	int usedMemory;
	int freeMemory;
	int mallocQty;
	int freeQty;
} memoryInfo;

typedef struct memoryBlock {
	uint64_t *base;
	uint64_t *current;
	size_t size;
} memoryBlock;

typedef struct PCB {
	processStatus status;
	int retValue;
	int childRetValue;
	int16_t pid;
	int16_t parentPid;
	int16_t pidToWait;
	uint16_t quantum;
	uint8_t priority;
	uint8_t isForeground;
	char *name;
	char **argv;
	memoryBlock *stack;
} PCB;

typedef struct processInfo {
	uint16_t pid;
	uint16_t parentPid;
	char *name;
	uint64_t *base;
	uint64_t *current;
	uint8_t priority;
	uint8_t isForeground;
	processStatus status;
} processInfo;

#endif