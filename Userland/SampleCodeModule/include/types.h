#ifndef _TYPES_H_U_
#define _TYPES_H_U_

#include <stddef.h>

typedef enum status {
	INVALID_PID = -8,
	SAME_STATUS,
	INVALID_STATUS,
	SAME_PRIORITY,
	INVALID_PRIORITY,
	NOT_FOUND,
	INVALID_PROCESS,
	PROBLEM,
	SUCCESS
} status;

typedef enum processStatus { RUNNING, BLOCKED, KILLED, READY, ZOMBIE } processStatus;

typedef int (*ProcessCode)(int argc, char ** args);

typedef struct memoryInfo {
	int totalMemory;
	int usedMemory;
	int freeMemory;
} memoryInfo;

typedef struct memoryBlock {
	uint64_t * base;
	uint64_t * current;
	size_t size;
} memoryBlock;

typedef struct PCB {
	int16_t pid;
	int16_t parentPid;
	int16_t pidToWait;
	char * name;
	char ** argv;
	memoryBlock * stack;
	uint8_t priority;
	uint16_t quantum;
	processStatus status;
	int retValue;
	int childRetValue;
} PCB;

// 3 (stdin, stdout, stderr)
#define STDIN 0
#define STDOUT 1
#define STDERR 2
#define STD_PIPES 3
#define READ	  0
#define WRITE	  1
#define EOF	  -1

typedef struct processInfo {
  uint16_t	pid;
  uint16_t	parentPid;
  char	       *name;
  uint64_t     *base;
  uint64_t     *current;
  uint8_t	priority;
  processStatus status;
  // todo agregar FOREGROUND o BACKGROUND
} processInfo;

#endif