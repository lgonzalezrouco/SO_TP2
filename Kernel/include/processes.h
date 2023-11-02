#ifndef _PROCESSES_H_
#define _PROCESSES_H_

#include "MemoryManager.h"
#include "lib.h"

#define STACK_SIZE 4096

typedef enum processStatus { RUNNING, BLOCKED, KILLED, READY } processStatus;

typedef struct PCB {
  uint16_t	pid;
  uint16_t	parentPid;
  char	       *name;
  void	       *basePointer;
  void	       *stackPointer;
  uint8_t	priority;
  processStatus status;
} PCB;

int createProcess(uint16_t parentPid, int (*func)(int, char **), char **args,
		  char *name, uint8_t priority);

void *setup_stack(void *wrapper, void *rip, void *rsp, void *args);

#endif