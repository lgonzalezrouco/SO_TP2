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
}

int createProcess(int (*func)(int, char **), char **args, char *name,
		  uint8_t priority) {
  if (processesQty >= MAX_PROCESSES)  // TODO: Agregar panic?
    return -1;
  PCB *process = (PCB *)malloc(sizeof(PCB));
  initializeProcess(process, nextPid, currentProcess->pid, func, args, name,
		    priority);
}

void *schedule(void *currentSP) {
  quantumRemaining--;
  if (!processesQty || quantumRemaining > 0)
    return currentSP;

  if (currentProcess != NULL) {
    if (currentProcess->status == RUNNING)
      currentProcess->status = READY;

    if (currentProcess->priority > 0)
      setPriority(currentProcess->pid, currentProcess->priority - 1);
  }
}

int setPriority(uint16_t pid, int newPriority) {
  PCB *process = processes[pid];
  if (process == NULL || pid == IDLE_PID)
    return -1;

  if (newPriority >= PRIORITY_LEVELS - 1 || newPriority < 0)
    return -1;

  if (process->status == READY || process->status == RUNNING) {
    // TODO: USAR QUEUE O LINKEDLIST?
  }
  process->priority = newPriority;
  return newPriority;
}