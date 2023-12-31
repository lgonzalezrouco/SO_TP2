// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <scheduler.h>

PCB *processes[MAX_PROCESSES];
PCB *currentProcess = NULL;
uint16_t quantumRemaining = 0;
QueueADT queues[PRIORITY_LEVELS];
int processesQty = 0;
uint8_t killForeground;

static PCB *getNextProcess();
static void killChildren(int16_t parentPid);
static void runProcess(int16_t pid);
static void stopProcess(int16_t pid);

void initializeScheduler() {
	resetPIDCounter();
	for (int i = 0; i < PRIORITY_LEVELS; i++)
		queues[i] = createQueueADT();
	for (int i = 0; i < MAX_PROCESSES; i++)
		processes[i] = NULL;
}

void startShell(int16_t pid) {
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

PCB *getProcess(int16_t pid) {
	return processes[pid];
}

int16_t getCurrentPid() {
	return currentProcess->pid;
}

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

	if (killForeground && currentProcess->isForeground) {
		killForeground = 0;
		if (currentProcess->pid != SHELL_PID && currentProcess->pid != IDLE_PID)
			killProcess(currentProcess->pid, -1);
	}

	PCB *nextProcess = getNextProcess();
	runProcess(nextProcess->pid);
	return currentProcess->stack->current;
}

int toggleBlockProcess(int16_t pid) {
	PCB *process = getProcess(pid);

	if (process == NULL)
		return NOT_FOUND;

	if (process->status == BLOCKED)
		return unblockProcess(pid);
	else {
		blockProcess(pid);
		if (process->pid == getCurrentPid())
			yield();
	}
	return SUCCESS;
}

int setPriority(int16_t pid, uint8_t newPriority) {
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

int getProcessesQty() {
	return processesQty;
}

int killCurrentProcess(int retValue) {
	return killProcess(getCurrentPid(), retValue);
}

int killProcess(int16_t pid, int retValue) {
	PCB *process = getProcess(pid);
	int16_t currentPid = getCurrentPid();

	if (process == NULL)
		return NOT_FOUND;

	if (process->pid == IDLE_PID || process->pid == SHELL_PID)
		return INVALID_PROCESS;

	process->retValue = retValue;

	killChildren(process->pid);
	if (process->status == READY || process->status == BLOCKED)
		removeByPid(queues[process->priority], process->pid);

	PCB *parent = getProcess(process->parentPid);
	if (parent->pidToWait == pid) {
		parent->childRetValue = retValue;
		unblockProcess(parent->pid);
	}

	for (int i = 0; i < STD_FDS; i++) {
		if (process->fds[i] >= STD_FDS)
			closePipe(process->fds[i], process->pid);
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

int waitpid(int16_t pid) {
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

int blockProcess(int16_t pid) {
	PCB *process = getProcess(pid);

	if (process == NULL)
		return NOT_FOUND;

	if (process->pid == IDLE_PID)
		return INVALID_PROCESS;

	if (process->status == BLOCKED)
		return SAME_STATUS;

	if (process->status == READY)
		removeByPid(queues[process->priority], process->pid);

	process->status = BLOCKED;
	process->priority = BLOCKED_PRIORITY;
	enqueue(queues[process->priority], process);

	return SUCCESS;
}

int unblockProcess(int16_t pid) {
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
		quantumRemaining = 0;
	}
	return SUCCESS;
}

int16_t getFileDescriptor(uint8_t index) {
	PCB *process = getProcess(getCurrentPid());
	return process->fds[index];
}

void killForegroundProcess() {
	killForeground = 1;
}

static PCB *getNextProcess() {
	PCB *process = NULL;

	for (int i = MAX_PRIORITY; i >= MIN_PRIORITY && process == NULL; i--) {
		if (!isEmpty(queues[i]))
			process = dequeue(queues[i]);
	}

	return process == NULL ? getProcess(IDLE_PID) : process;
}

static void killChildren(int16_t parentPid) {
	for (int i = 0; i < MAX_PROCESSES; i++) {
		if (processes[i] != NULL && processes[i]->parentPid == parentPid)
			killProcess(processes[i]->pid, -1);
	}
}

static void runProcess(int16_t pid) {
	PCB *process = getProcess(pid);
	process->status = RUNNING;
	quantumRemaining = process->quantum;
	currentProcess = process;
}

static void stopProcess(int16_t pid) {
	PCB *process = getProcess(pid);
	process->status = READY;
	removeByPid(queues[process->priority], process->pid);
	process->priority = process->priority > MIN_PRIORITY ? process->priority - 1 : process->priority;
	enqueue(queues[process->priority], process);
	process->quantum = process->quantum > MAX_QUANTUM ? process->quantum : process->quantum * 2;
}