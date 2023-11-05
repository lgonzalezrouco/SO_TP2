#include <scheduler.h>

#define MAX_PROCESSES 128
#define IDLE_PID      0

PCB	*processes[MAX_PROCESSES];
PCB	*currentProcess = NULL;
uint16_t quantumRemaining = 0;
QueueADT queues[PRIORITY_LEVELS];
int	 processesQty = 0;

static PCB *getNextProcess() {
  PCB	  *process = NULL;
  uint16_t nextPid = IDLE_PID;

  for (int i = PRIORITY_LEVELS - 1; i >= 0 && process == NULL; i--) {
    if (!isEmpty(queues[i]))
      process = dequeue(queues[i]);
  }

  if (process != NULL)
    nextPid = process->pid;

  return processes[nextPid];
}

void initializeScheduler() {
  for (int i = 0; i < PRIORITY_LEVELS; i++)
    queues[i] = createQueueADT();
  for (int i = 0; i < MAX_PROCESSES; i++)
    processes[i] = NULL;
}

void includeTerminal(uint16_t pid) {
  currentProcess = getProcess(pid);
  currentProcess->status = RUNNING;
  forceChangeOfProcess(currentProcess->stack->current);
}

void addProcess(PCB *process) {
  enqueue(queues[process->priority], process);
  processes[process->pid] = process;
  processesQty++;
}

PCB *removeProcess(PCB *process) {
  PCB *removed = removeByPid(queues[process->priority], process->pid);
  processes[process->pid] = NULL;
  processesQty--;
  return removed;
}

PCB *getProcess(uint16_t pid) { return processes[pid]; }

uint16_t getCurrentPid() { return currentProcess->pid; }

void *schedule(void *currentSP) {
  static int firstTime = 1;

  if (!processesQty || quantumRemaining > 0) {
    quantumRemaining--;
    return currentSP;
  }

  if (currentProcess != NULL) {
    if(!firstTime)
      currentProcess->stack->current = currentSP;
    else
      firstTime = 0;

    if (currentProcess->status == RUNNING)
      currentProcess->status = READY;

    uint16_t newPriority = currentProcess->priority > 0 ?
  currentProcess->priority -1 : currentProcess->priority;
    setPriority(currentProcess->pid, newPriority);
    // multiplica el quantum por 2 cada vez que se termina el quantum asignado
    currentProcess->quantum *= 2;
  }

  PCB *nextProcess = getNextProcess();
  quantumRemaining = nextProcess->quantum;
  nextProcess->status = RUNNING;
  currentProcess = nextProcess;
  return currentProcess->stack->current;
}

int setPriority(uint16_t pid, uint16_t newPriority) {
  PCB *process = processes[pid];
  if (process == NULL || pid == IDLE_PID)
    return -1;

  if (newPriority >= PRIORITY_LEVELS - 1 || newPriority < 0)
    return -1;

  if (process->status == READY || process->status == RUNNING) {
    dequeue(queues[process->priority]);
    enqueue(queues[newPriority], process);
  }

  process->priority = newPriority;
  return newPriority;
}