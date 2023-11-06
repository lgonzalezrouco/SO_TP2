#ifndef _TYPES_H_
#define _TYPES_H_

#include <stddef.h>
#include <stdint.h>

#define INVALID_PROCESS	 -1
#define NOT_FOUND	 -2
#define INVALID_PRIORITY -3
#define SAME_PRIORITY	 -4

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
  char	      **argv;
  memoryBlock  *stack;
  uint8_t	priority;
  uint16_t	quantum;
  processStatus status;
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