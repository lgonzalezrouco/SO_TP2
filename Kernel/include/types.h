#ifndef _TYPES_H_
#define _TYPES_H_

#include <stddef.h>
#include <stdint.h>

typedef enum processStatus {
  RUNNING,
  BLOCKED,
  KILLED,
  READY,
  ZOMBIE
} processStatus;

typedef int (*ProcessCode)(int argc, char **args);

typedef struct memoryBlock {
  uint64_t *base;
  uint64_t *current;
  size_t    size;
} memoryBlock;

typedef struct PCB {
  uint16_t	pid;
  uint16_t	parentPid;
  char	       *name;
  memoryBlock  *stack;
  uint16_t	priority;
  uint16_t	quantum;
  processStatus status;
} PCB;

#endif