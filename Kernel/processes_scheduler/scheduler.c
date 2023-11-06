#include <scheduler.h>

PCB	*processes[MAX_PROCESSES];
PCB	*currentProcess = NULL;
int16_t	 quantumRemaining = 0;
QueueADT queues[PRIORITY_LEVELS];
int	 processesQty = 0;

static PCB *getNextProcess() {
  PCB	  *process = NULL;
  uint16_t nextPid = IDLE_PID;

  for (int i = MAX_PRIORITY; i >= MIN_PRIORITY && process == NULL; i--) {
    if (!isEmpty(queues[i]))
      process = dequeue(queues[i]);
  }

  if (process != NULL)
    nextPid = process->pid;

  return processes[nextPid];
}

void initializeScheduler() {
  resetPIDCounter();
  for (int i = 0; i < PRIORITY_LEVELS; i++)
    queues[i] = createQueueADT();
  for (int i = 0; i < MAX_PROCESSES; i++)
    processes[i] = NULL;
}

void startShell(uint16_t pid) {
  currentProcess = getProcess(pid);
  currentProcess->status = RUNNING;
  quantumRemaining = currentProcess->quantum;
  removeByPid(queues[currentProcess->priority], currentProcess->pid);
  forceChangeOfProcess(currentProcess->stack->current);
}

void addProcess(PCB *process) {
  enqueue(queues[process->priority], process);
  processes[process->pid] = process;
  processesQty++;
}

PCB *removeProcess(PCB *process) {
  if (process == NULL)
    return NOT_FOUND;
  if (process->status != ZOMBIE && process->status != BLOCKED)
    removeByPid(queues[process->priority], process->pid);
  PCB *removed = processes[process->pid];
  processes[process->pid] = NULL;
  processesQty--;
  return removed;
}

PCB *getProcess(uint16_t pid) { return processes[pid]; }

uint16_t getCurrentPid() { return currentProcess->pid; }

void *schedule(void *currentSP) {
  /* static int firstTime = 1;

  if (!processesQty || quantumRemaining > 0) { // no termino el quantum
    quantumRemaining--;
    return currentSP;
  }

  if (currentProcess != NULL) {
    if (!firstTime)
      currentProcess->stack->current = currentSP;
    else
      firstTime = 0;

    if (currentProcess->status == RUNNING)
      currentProcess->status = READY;

    uint8_t newPriority = currentProcess->priority > 1 ?
			       currentProcess->priority - 1 :
			       currentProcess->priority;
    setPriority(currentProcess->pid, newPriority);
    // multiplica el quantum por 2 cada vez que se termina el quantum asignado
    currentProcess->quantum *= 2;
  } */

  if (quantumRemaining > 0 && currentProcess->status != ZOMBIE &&
      currentProcess->status != BLOCKED && currentProcess->status != KILLED) {
    quantumRemaining--;
    return currentSP;
  }

  stopProcess(currentSP);
  PCB *nextProcess = getNextProcess();
  quantumRemaining = nextProcess->quantum;
  nextProcess->status = RUNNING;
  currentProcess = nextProcess;
  return currentProcess->stack->current;
}

void stopProcess(void *currentSP) {
  if (currentSP == NULL)
    return;

  currentProcess->stack->current = currentSP;
  if (currentProcess->pid == IDLE_PID)
    return;

  if (currentProcess->status == RUNNING) {
    currentProcess->status = READY;
    currentProcess->priority = currentProcess->priority > 1 ?
				   currentProcess->priority - 1 :
				   currentProcess->priority;
    enqueue(queues[currentProcess->priority], currentProcess);
    currentProcess->quantum *= 2;
  }
}

int setPriority(uint16_t pid, uint8_t newPriority) {
  PCB *process = processes[pid];

  if (process == NULL)
    return NOT_FOUND;

  if (pid == IDLE_PID)
    return INVALID_PROCESS;

  if (process->priority == newPriority)
    return SAME_PRIORITY;

  if (newPriority > MAX_PRIORITY || newPriority < MIN_PRIORITY)
    return INVALID_PRIORITY;

  if (process->status == READY || process->status == RUNNING) {
    removeByPid(queues[process->priority], process->pid);
    enqueue(queues[newPriority], process);
  }

  process->priority = newPriority;
  return newPriority;
}

int setStatus(uint16_t pid, processStatus newStatus) {
  PCB *process = processes[pid];
  if (process == NULL || pid == IDLE_PID || process->status == ZOMBIE)
    return -1;

  if (newStatus > 4 || newStatus < 0)
    return -1;

  if (process->status == newStatus)
    return newStatus;

  switch (newStatus) {
    case KILLED: removeProcess(process); break;
    case BLOCKED:  // todo ver bien que hacer con los bloqueados
    case ZOMBIE: removeByPid(queues[process->priority], process->pid); break;
    case RUNNING: break;
    case READY: break;
    default: break;
  }

  process->status = newStatus;
  return newStatus;
}

int getProcessesQty() { return processesQty; }

int killProcess(uint16_t pid, int retValue) {
  PCB	  *process = getProcess(pid);
  uint16_t currentPid = getCurrentPid();

  if (process == NULL)
    return NOT_FOUND;

  if (process->status == ZOMBIE || process->pid == IDLE_PID ||
      process->pid == SHELL_PID)
    return INVALID_PROCESS;

  if (process->status == READY || process->status == BLOCKED)
    removeByPid(queues[process->priority], process->pid);

  process->status = ZOMBIE;
  process->retValue = retValue;

  // elimina a todos los hijos zombies del proceso killeado
  begin(process->zombieChildren);
  while (hasNext(process->zombieChildren)) {
    PCB *child = (PCB *)next(process->zombieChildren);
    removeNode(process->zombieChildren, child);
    processes[child->pid] = NULL;
    processesQty--;
    freeProcess(child);
  }

  // El proceso killeado se agrega a la lista de zombies del padre y si el padre
  // hizo waitpid de este, se desbloquea
  PCB *parent = getProcess(process->parentPid);
  if (parent != NULL && parent->status != ZOMBIE) {
    if (parent->pidToWait == process->pid && parent->status == BLOCKED) {
      parent->pidToWait = -1;
      setStatus(parent->pid, READY);
      processes[process->pid] = NULL;
      processesQty--;
      freeProcess(process);

    } else
      addProcessAtLast(parent->zombieChildren, process);
  } else {
    // removeNode(parent->zombieChildren, process);
    processesQty--;
    processes[process->pid] = NULL;
    freeProcess(process);
  }

  if (pid == currentPid)
    yield();

  return 0;
}

void yield() {
  quantumRemaining = 0;
  forceTimerTick();
}