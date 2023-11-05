#include <processes.h>

static uint16_t nextPid = 0;

static void   freeProcess(PCB *process);
static char **allocArguments(char **args);
static void freeProcessInfo(processInfo *info);

void resetPIDCounter() { nextPid = 0; }

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
  process->status = READY;
  process->quantum = 1;
  process->priority = priority;

  process->argv = allocArguments(args);

  void *stackEnd = (void *)((uint64_t)process->stack->base + STACK_SIZE);

  process->stack->current =
      setup_stack(&processWrapper, code, stackEnd, (void *)process->argv);

  addProcess(process);

  return process->pid;
}

int killProcess(uint16_t pid) {
  PCB *process = getProcess(pid);

  if (process == NULL || process->pid == IDLE_PID || process->pid == SHELL_PID)
    return -1;

  if (getProcess(process->parentPid) == NULL || process->status == ZOMBIE) {
    setStatus(pid, KILLED);
    freeProcess(process);
  } else
    setStatus(pid, ZOMBIE);

  if (process->pid == getCurrentPid())
    forceTimerTick();

  return 0;
}

int killCurrentProcess() { return killProcess(getCurrentPid()); }

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

static char **allocArguments(char **args) {
  int argc = array_strlen(args), totalArgsLen = 0;
  int argsLen[argc];
  for (int i = 0; i < argc; i++) {
    argsLen[i] = strlen(args[i]) + 1;
    totalArgsLen += argsLen[i];
  }
  char **newArgsArray =
      (char **)malloc(totalArgsLen + sizeof(char **) * (argc + 1));
  char *charPosition = (char *)newArgsArray + (sizeof(char **) * (argc + 1));
  for (int i = 0; i < argc; i++) {
    newArgsArray[i] = charPosition;
    memcpy(charPosition, args[i], argsLen[i]);
    charPosition += argsLen[i];
  }
  newArgsArray[argc] = NULL;
  return newArgsArray;
}

processInfo *getProcessInfo(PCB *process) {
  processInfo *info = malloc(sizeof(processInfo));
  if (info == NULL)
    return NULL;
  info->pid = process->pid;
  info->parentPid = process->parentPid;
  info->name = malloc(strlen(process->name) + 1);
  if (info->name == NULL) {
    free(info);
    return NULL;
  }
  strcpy(info->name, process->name);
  info->base = process->stack->base;
  info->current = process->stack->current;
  info->priority = process->priority;
  info->status = process->status;
  return info;
}

processInfo **getProcessesInfo() {
  processInfo **info = malloc(sizeof(processInfo *) * (getProcessesQty() + 1));
  int		i = 0;
  PCB	       *process = NULL;
  for (int j = 0; j < MAX_PROCESSES; j++) {
    process = getProcess(j);
    if (process != NULL)
      info[i++] = getProcessInfo(process);
  }
  info[i] = NULL;
  return info;
}

static void freeProcessInfo(processInfo *info) {
  free(info->name);
  free(info);
}

void freeProcessesInfo(processInfo **infoArray) {
  int i = 0;
  while (infoArray[i] != NULL) {
    freeProcessInfo(infoArray[i]);
    i++;
  }
  free(infoArray);
}
