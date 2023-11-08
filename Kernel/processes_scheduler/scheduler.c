// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <scheduler.h>

PCB	*processes[MAX_PROCESSES];
PCB	*currentProcess = NULL;
uint16_t quantumRemaining = 0;
QueueADT queues[PRIORITY_LEVELS];
int	 processesQty = 0;

static PCB *getNextProcess();
static void killChildren(uint16_t parentPid);
static int  blockProcess(uint16_t pid);
static int  unblockProcess(uint16_t pid);
static void runProcess(uint16_t pid);
static void stopProcess(uint16_t pid);

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
    stopProcess(currentProcess->pid);

  PCB *nextProcess = getNextProcess();
  runProcess(nextProcess->pid);
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

  if (newPriority > MAX_PRIORITY || newPriority < MIN_PRIORITY)
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

int killProcess(uint16_t pid, int retValue) {
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
  /*     else
	  currentProcess = NULL; */

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

  blockProcess(parent->pid);

  if (child->parentPid == getCurrentPid())
    yield();

  return parent->childRetValue;
}

int16_t getNextPid() {
  for (int i = 0; i < MAX_PROCESSES; i++) {
    if (processes[i] == NULL)
      return i;
  }
  return INVALID_PID;
}

static PCB *getNextProcess() {
  PCB *process = NULL;

  for (int i = MAX_PRIORITY; i >= MIN_PRIORITY && process == NULL; i--) {
    if (!isEmpty(queues[i]))
      process = dequeue(queues[i]);
  }

  return process == NULL ? getProcess(IDLE_PID) : process;
}

static void killChildren(uint16_t parentPid) {
  for (int i = 0; i < MAX_PROCESSES; i++) {
    if (processes[i] != NULL && processes[i]->parentPid == parentPid)
      killProcess(processes[i]->pid, -1);
  }
}

static int blockProcess(uint16_t pid) {
  PCB *process = getProcess(pid);

  if (process == NULL)
    return NOT_FOUND;

  if (process->status == BLOCKED)
    return SAME_STATUS;
  else if (process->status == RUNNING) {
    process->status = BLOCKED;
    process->priority = BLOCKED_PRIORITY;
    enqueue(queues[process->priority], process);
    yield();
  } else if (process->status == READY) {
    removeByPid(queues[process->priority], process->pid);
    process->status = BLOCKED;
    process->priority = BLOCKED_PRIORITY;
    enqueue(queues[process->priority], process);
  }
  return SUCCESS;
}

static int unblockProcess(uint16_t pid) {
  PCB *process = getProcess(pid);

  if (process == NULL)
    return NOT_FOUND;
  if (process->status == READY || process->status == RUNNING)
    return SAME_STATUS;
  else if (process->status == BLOCKED) {
    removeByPid(queues[process->priority], process->pid);
    process->priority = MAX_PRIORITY;
    enqueue(queues[process->priority], process);
    process->status = READY;
    process->quantum = 1;
  }
  return SUCCESS;
}

static void runProcess(uint16_t pid) {
  PCB *process = getProcess(pid);
  process->status = RUNNING;
  quantumRemaining = process->quantum;
  currentProcess = process;
}

static void stopProcess(uint16_t pid) {
  PCB *process = getProcess(pid);
  process->status = READY;
  removeByPid(queues[process->priority], process->pid);
  process->priority = process->priority > MIN_PRIORITY ? process->priority - 1 :
							 process->priority;
  enqueue(queues[process->priority], process);
  process->quantum =
      process->quantum > MAX_QUANTUM ? process->quantum : process->quantum * 2;
}