#ifndef _TYPES_H_U_
#define _TYPES_H_U_

#include <stddef.h>

typedef enum status {
  INVALID_PID = -7,
  SAME_STATUS,
  INVALID_STATUS,
  SAME_PRIORITY,
  INVALID_PRIORITY,
  NOT_FOUND,
  INVALID_PROCESS,
  SUCCESS
} status;

typedef enum processStatus {
  RUNNING,
  BLOCKED,
  KILLED,
  READY,
  ZOMBIE
} processStatus;

typedef int (*ProcessCode)(int argc, char **args);

typedef struct memoryInfo {
  int totalMemory;
  int usedMemory;
  int freeMemory;
} memoryInfo;

typedef struct memoryBlock {
  uint64_t *base;
  uint64_t *current;
  size_t    size;
} memoryBlock;

typedef struct PCB {
  uint16_t	pid;
  uint16_t	parentPid;
  uint16_t	pidToWait;
  char	       *name;
  char	      **argv;
  memoryBlock  *stack;
  uint8_t	priority;
  uint16_t	quantum;
  processStatus status;
  int		retValue;
  int		childRetValue;
} PCB;

#endif