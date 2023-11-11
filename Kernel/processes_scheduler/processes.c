// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <processes.h>

static int16_t nextPid = 0;

void resetPIDCounter() {
	nextPid = 0;
}

void processWrapper(ProcessCode function, char **args) {
	size_t len = array_strlen(args);
	int retValue = function(len, args);
	killProcess(getCurrentPid(), retValue);
}

int createProcess(int16_t parentPid, ProcessCode code, char **args, char *name, uint8_t priority, int fds[]) {
	PCB *process = (PCB *) malloc(sizeof(PCB));
	if (process == NULL)
		return -1;

	process->pid = getNextPid();
	if (process->pid == INVALID_PID) {
		free(process);
		return -1;
	}

	process->parentPid = parentPid;

	process->pidToWait = -1;

	process->name = malloc(strlen(name) + 1);
	if (process->name == NULL) {
		free(process);
		return -1;
	}
	strcpy(process->name, name);

	process->stack = (memoryBlock *) malloc(sizeof(memoryBlock));
	if (process->stack == NULL) {
		free(process->name);
		free(process);
		return -1;
	}

	process->stack->base = (uint64_t *) malloc(STACK_SIZE);
	if (process->stack->base == NULL) {
		free(process->name);
		free(process->stack);
		free(process);
		return -1;
	}
	process->stack->size = STACK_SIZE;
	process->status = READY;
	process->quantum = 1;
	priority = priority > MAX_PRIORITY ? MAX_PRIORITY : priority;
	process->priority = priority;
	process->retValue = -1;
	process->childRetValue = -1;
	process->argv = args;

	void *stackEnd = (void *) ((uint64_t) process->stack->base + STACK_SIZE);

	process->stack->current = setup_stack(&processWrapper, code, stackEnd, (void *) process->argv);

	addProcess(process);

	process->fds[STDIN] = fds[STDIN];
	process->fds[STDOUT] = fds[STDOUT];
	process->fds[STDERR] = fds[STDERR];

	return process->pid;
}

int idle(int argc, char **argv) {
	while (1)
		_hlt();

	return 0;
}

void freeProcess(PCB *process) {
	free(process->stack->base);
	free(process->stack);
	free(process->name);
	free(process);
}

PCB **getProcessesInfo() {
	PCB **info = malloc(sizeof(PCB *) * (getProcessesQty() + 1));
	if (info == NULL)
		return NULL;

	int i = 0;
	PCB *process = NULL;
	for (int j = 0; j < MAX_PROCESSES; j++) {
		process = getProcess(j);
		if (process != NULL)
			info[i++] = process;
	}

	info[i] = NULL;
	return info;
}

void freeProcessesInfo(PCB **infoArray) {
	free(infoArray);
}