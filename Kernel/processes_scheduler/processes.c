#include <processes.h>

static uint16_t nextPid = 0;

static void freeProcess(PCB *process);

void processWrapper(ProcessCode function, char **args) {
  size_t len = array_strlen(args);
  function(len, args);
  killProcess(getCurrentPid());
}

int createProcess(uint16_t parentPid, ProcessCode code, char **args, char *name,
		  uint8_t priority) {
  PCB *process = (PCB *)malloc(sizeof(PCB));
  if (process == NULL)
    return -1;

  process->pid = nextPid;
  nextPid++;

  process->parentPid = parentPid;

  process->name = malloc(strlen(name) + 1);
  if (process->name == NULL) {
    free(process);
    return -1;
  }
  strcpy(process->name, name);

  process->stack = (memoryBlock *)malloc(sizeof(memoryBlock));
  if (process->stack == NULL) {
    free(process->name);
    free(process);
    return -1;
  }

  process->stack->base = (uint64_t *)malloc(STACK_SIZE);
  if (process->stack->base == NULL) {
    free(process->name);
    free(process->stack);
    free(process);
    return -1;
  }
  process->stack->size = STACK_SIZE;
  process->stack->current = process->stack->base + process->stack->size;
  process->status = READY;
  process->quantum = 1;
  process->priority = priority;
  process->stack->current =
      setup_stack((uint64_t *)process->stack->base + process->stack->size, code,
		  args, &processWrapper);

  addProcess(process);

  return process->pid;
}

int killProcess(uint16_t pid) {
  PCB *process = getProcess(pid);

  if (process == NULL)
    return -1;

  if (getProcess(process->parentPid) == NULL || process->status == ZOMBIE) {
    process->status = KILLED;
    removeProcess(process);
    freeProcess(process);
  } else
    process->status = ZOMBIE;

  if (process->pid == getCurrentPid())
    forceTimerTick();

  return 0;
}

int idle(int argc, char **argv) {
  while (1)
    _hlt();

  return 0;
}

static void freeProcess(PCB *process) {
  free(process->stack->base);
  free(process->stack);
  free(process->name);
  free(process);
}