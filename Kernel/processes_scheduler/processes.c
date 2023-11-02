#include <processes.h>

void processWrapper(int (*func)(int, char **), char **args) {
  size_t len = array_strlen(args);
  int	 retValue = func(len, args);
  // killCurrentProcess(retValue);
}

int initializeProcess(PCB *process, uint16_t pid, uint16_t parentPid,
		      int (*func)(int, char **), char **args, char *name,
		      uint8_t priority) {
  process->pid = pid;
  process->parentPid = parentPid;

  process->basePointer = malloc(STACK_SIZE);
  if (process->basePointer == NULL) {
    free(process);
    return -1;
  }

  process->name = malloc(strlen(name) + 1);
  if (process->name == NULL) {
    free(process->basePointer);
    free(process);
    return -1;
  }
  strcpy(process->name, name);

  process->priority = priority;

  process->status = READY;

  process->quantum = 1;

  void *stackEnd = (void *)((uint64_t)process->basePointer + STACK_SIZE);

  process->stackPointer = setup_stack(&processWrapper, func, stackEnd, args);

  return 0;
}

/* int createProcess(uint16_t parentPid, int (*func)(int, char **), char **args,
		  char *name, uint8_t priority) {
  PCB *process = (PCB *)malloc(sizeof(PCB));
  if (process == NULL)
    return -1;

  process->pid = nextPid;
  nextPid++;

  process->parentPid = parentPid;

  process->basePointer = malloc(STACK_SIZE);
  if (process->basePointer == NULL) {
    free(process);
    return -1;
  }

  process->name = malloc(strlen(name) + 1);
  if (process->name == NULL) {
    free(process->basePointer);
    free(process);
    return -1;
  }
  strcpy(process->name, name);

  process->priority = priority;

  process->status = READY;

  void *stackEnd = (void *)((uint64_t)process->basePointer + STACK_SIZE);

  process->stackPointer = setup_stack(&processWrapper, func, stackEnd, args);

  addProcessToQueue(process);

  return process->pid;
} */