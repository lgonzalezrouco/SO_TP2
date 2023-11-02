#include <scheduler.h>

#define MAX_PROCESSES 128
#define IDLE_PID      0

static uint16_t nextPid = 0;

PCB	*processes[MAX_PROCESSES];
PCB	*currentProcess = NULL;
int	 quantumRemaining = 0;
QueueADT queues[PRIORITY_LEVELS];
int	 processesQty = 0;

static PCB *getNextProcess() {
  PCB *process = NULL;
  int  nextPid = IDLE_PID;

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
  for (int i = 0; i < MAX_PROCESSES; i++)  // todo esto es necesario?
    processes[i] = NULL;
}

int createProcess(int (*func)(int, char **), char **args, char *name,
		  uint8_t priority) {
  if (processesQty >= MAX_PROCESSES)
    return -1;
  PCB *process = (PCB *)malloc(sizeof(PCB));
  if (initializeProcess(process, nextPid,
			currentProcess == NULL ? 0 : currentProcess->pid, func,
			args, name, priority) != 0)
    return -1;

  enqueue(queues[priority], process);
  processes[nextPid] = process;

  while (processes[nextPid] != NULL)
    nextPid = (nextPid + 1) % MAX_PROCESSES;

  processesQty++;

  return process->pid;
}

void *schedule(void *currentSP) {
  quantumRemaining--;
  if (!processesQty || quantumRemaining > 0)
    return currentSP;

  if (currentProcess != NULL) {
    if (currentProcess->status == RUNNING)
      currentProcess->status = READY;
    setPriority(currentProcess->pid, currentProcess->priority - 1);
    // multiplica el quantum por 2 cada vez que se termina el quantum asignado
    currentProcess->quantum *= 2;
  }

  PCB *nextProcess = getNextProcess();
  quantumRemaining = nextProcess->quantum;
  nextProcess->status = RUNNING;
  currentProcess = nextProcess;
  return currentProcess->stackPointer;
}

int setPriority(uint16_t pid, int newPriority) {
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