#ifndef _TYPES_H_
#define _TYPES_H_

#include <stddef.h>
#include <stdint.h>

typedef enum status {
  SAME_STATUS = -6,
  INVALID_STATUS,
  SAME_PRIORITY,
  INVALID_PRIORITY,
  NOT_FOUND,
  INVALID_PROCESS,
  SUCCESS
} status;

#define PRIORITY_LEVELS	 7
#define BLOCKED_PRIORITY (PRIORITY_LEVELS - 1)
#define MAX_PRIORITY	 (PRIORITY_LEVELS - 2)
#define MIN_PRIORITY	 1
#define IDLE_PRIORITY	 0

typedef enum processStatus { RUNNING, BLOCKED, KILLED, READY } processStatus;

typedef int (*ProcessCode)(int argc, char **args);

// todo cambiar los tipos de pid por pid_t

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