#include <scheduler.h>
#include <video.h>

PCB	*processes[MAX_PROCESSES];
PCB	*currentProcess = NULL;
uint16_t quantumRemaining = 0;
QueueADT queues[PRIORITY_LEVELS];
int	 processesQty = 0;

static PCB *getNextProcess();
static int  setStatus(uint16_t pid, processStatus newStatus);
static void killChildren(uint16_t parentPid);
static int blockProcess(uint16_t pid);
static int unblockProcess(uint16_t pid);

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

PCB *getProcess(uint16_t pid) { return processes[pid]; }

uint16_t getCurrentPid() { return currentProcess->pid; }

void *schedule(void *currentSP) {
  if (quantumRemaining > 0) {
    quantumRemaining--;
    return currentSP;
  }

  if (currentSP == NULL)
    return NULL;

  currentProcess->stack->current = currentSP;

  if (currentProcess->status != BLOCKED)
    setStatus(currentProcess->pid, READY);

  PCB *nextProcess = getNextProcess();
  setStatus(nextProcess->pid, RUNNING);
  return currentProcess->stack->current;
}

int toggleBlockProcess(uint16_t pid) {
  PCB *process = getProcess(pid);

  if (process == NULL)
    return NOT_FOUND;

  if (process->status == BLOCKED)
    return unblockProcess(pid);
  else
    return blockProcess(pid);
}

int setPriority(uint16_t pid, uint8_t newPriority) {
  PCB *process = getProcess(pid);

  if (process == NULL)
    return NOT_FOUND;

  if (pid == IDLE_PID)
    return INVALID_PROCESS;

  if (process->priority == newPriority && process->status != RUNNING)
    return SAME_PRIORITY;

  if (newPriority > (PRIORITY_LEVELS - 1) || newPriority < MIN_PRIORITY)
    return INVALID_PRIORITY;

  if (process->status == READY || process->status == BLOCKED)
    removeByPid(queues[process->priority], process->pid);

  enqueue(queues[newPriority], process);

  process->priority = newPriority;
  return newPriority;
}

int getProcessesQty() { return processesQty; }

int killCurrentProcess(int retValue) {
  return killProcess(getCurrentPid(), retValue);
}

int killProcess(uint16_t pid, int	retValue) {
  PCB	  *process = getProcess(pid);
  uint16_t currentPid = getCurrentPid();

  if (process == NULL)
    return NOT_FOUND;

  if (process->pid == IDLE_PID || process->pid == SHELL_PID)
    return INVALID_PROCESS;

  process->retValue = retValue;

  killChildren(process->pid);
  if (process->status == READY || process->status == BLOCKED)
    removeByPid(queues[process->priority], process->pid);
  else
    currentProcess = NULL;
    
  PCB *parent = getProcess(process->parentPid);
  if (parent->pidToWait == pid) {
    parent->childRetValue = retValue;
    unblockProcess(parent->pid);
  }

  processes[process->pid] = NULL;
  processesQty--;
  freeProcess(process);

  if (pid == currentPid)
    yield();
    
  return 0;
}

void yield() {
  quantumRemaining = 0;
  forceTimerTick();
}

int waitpid(uint16_t pid) {
  PCB *child = getProcess(pid);

  if (child == NULL)
    return NOT_FOUND;

  PCB *parent = getProcess(child->parentPid);
  parent->pidToWait = pid;

  setStatus(child->parentPid, BLOCKED);

  if (child->parentPid == getCurrentPid())
    yield();

  return parent->childRetValue;
}

int16_t getNextPid () {
  for (int i = 0; i < MAX_PROCESSES; i++) {
    if (processes[i] == NULL)
      return i;
  }
  return INVALID_PID;
}

static int blockProcess(uint16_t pid) {
  PCB *process = getProcess(pid);

  if (process == NULL)
    return NOT_FOUND;

  if (process->status == BLOCKED)
    return unblockProcess(pid);
  else if (process->status == RUNNING) {
    int result = setStatus(pid, BLOCKED);
    yield();
    return result;
  } else if (process->status == READY)
    return setStatus(pid, BLOCKED);
  return SUCCESS;
}

static int unblockProcess(uint16_t pid) {
  PCB *process = getProcess(pid);

  if (process == NULL)
    return NOT_FOUND;

  if (process->status == READY || process->status == RUNNING)
    return SAME_STATUS;
  else if (process->status == BLOCKED)
    return setStatus(pid, READY);
  return SUCCESS;
}

static void killChildren(uint16_t parentPid) {
  for (int i = 0; i < MAX_PROCESSES; i++) {
    if (processes[i] != NULL && processes[i]->parentPid == parentPid)
      killProcess(processes[i]->pid, -1);
  }
}

static PCB *getNextProcess() {
  PCB	  *process = NULL;
  uint16_t nextPid = IDLE_PID;

  for (int i = MAX_PRIORITY; i >= MIN_PRIORITY && process == NULL; i--) {
    if (!isEmpty(queues[i]))
      process = dequeue(queues[i]);
  }

  if (process != NULL)
    nextPid = process->pid;

  return getProcess(nextPid);
}

static int setStatus(uint16_t pid, processStatus newStatus) {
  PCB *process = getProcess(pid);

  if (process == NULL)
    return INVALID_PROCESS;

  if (newStatus > 4 || newStatus < 0)
    return INVALID_STATUS;

  if (process->status == newStatus)
    return SAME_STATUS;

  if (process->status == READY) {
    switch (newStatus) {
      case RUNNING:
	quantumRemaining = process->quantum;
	currentProcess = process;
	break;
      case BLOCKED: setPriority(process->pid, BLOCKED_PRIORITY); break;
      default: break;
    }
  } else if (process->status == RUNNING) {
    switch (newStatus) {
      case READY:
	setPriority(process->pid, process->priority > MIN_PRIORITY ?
				      process->priority - 1 :
				      process->priority);
	process->quantum = process->quantum > MAX_QUANTUM ?
			       process->quantum :
			       process->quantum * 2;
	break;
      case BLOCKED: setPriority(process->pid, BLOCKED_PRIORITY); break;
      default: break;
    }
  } else if (process->status == BLOCKED) {
    switch (newStatus) {
      case READY:
	setPriority(process->pid, MAX_PRIORITY);
	process->quantum = 1;
	break;
      default: return INVALID_STATUS; break;
    }
  }

  process->status = newStatus;
  return newStatus;
}