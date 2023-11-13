// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <processes.h>

static int16_t nextPid = 0;
static processInfo *getProcessInfo();

void resetPIDCounter() {
	nextPid = 0;
}

void processWrapper(ProcessCode function, char **args) {
	size_t len = array_strlen(args);
	int retValue = function(len, args);
	killProcess(getCurrentPid(), retValue);
}

int createProcess(ProcessCode code, char **args, char *name, uint8_t isForeground, int fds[]) {
	PCB *process = (PCB *) malloc(sizeof(PCB));
	if (process == NULL)
		return -1;
	process->pid = getNextPid();
	if (process->pid == INVALID_PID) {
		free(process);
		return -1;
	}

	process->parentPid = getCurrentPid();

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
	process->priority = process->pid == IDLE_PRIORITY ? IDLE_PRIORITY : MAX_PRIORITY;
	process->isForeground = isForeground;
	process->retValue = -1;
	process->childRetValue = -1;

	size_t len = array_strlen(args);
	process->argv = (char **) malloc(sizeof(char *) * (len + 1));
	for (int i = 0; i < len; i++) {
		process->argv[i] = malloc(strlen(args[i]) + 1);
		if (process->argv[i] == NULL) {
			free(process->stack->base);
			free(process->stack);
			free(process->name);
			for (int j = 0; j < i; j++)
				free(process->argv[j]);
			free(process->argv);
			free(process);
			return -1;
		}
		strcpy(process->argv[i], args[i]);
	}
	process->argv[len] = NULL;

	void *stackEnd = (void *) ((uint64_t) process->stack->base + STACK_SIZE);

	process->stack->current = setup_stack(&processWrapper, code, stackEnd, (void *) process->argv);

	addProcess(process);

	for (int i = 0; i < STD_FDS; i++) {
		process->fds[i] = fds[i];
		if (fds[i] >= STD_FDS) {
			if (i == STDIN)
				openPipe(fds[i], READ, process->pid);
			else
				openPipe(fds[i], WRITE, process->pid);
		}
	}

	return process->pid;
}

int idle(int argc, char **argv) {
	while (1)
		_hlt();

	return 0;
}

void freeProcess(PCB *process) {
	for (int i = 0; process->argv[i] != NULL; i++)
		free(process->argv[i]);
	free(process->argv);
	free(process->stack->base);
	free(process->stack);
	free(process->name);
	free(process);
}

processInfo **getProcessesInfo() {
	processInfo **info = malloc(sizeof(processInfo *) * (getProcessesQty() + 1));
	if (info == NULL)
		return NULL;

	int i = 0;
	PCB *process = NULL;
	for (int j = 0; j < MAX_PROCESSES; j++) {
		process = getProcess(j);
		if (process != NULL){
			processInfo *pInfo = getProcessInfo(process);
			info[i++] = pInfo;
		}
	}

	info[i] = NULL;
	return info;
}

void freeProcessesInfo(processInfo **infoArray) {
	for (int i = 0; infoArray[i] != NULL; i++) {
		free(infoArray[i]->name);
		free(infoArray[i]);
	}
	free(infoArray);
}

uint16_t getFdValue(uint8_t fdIndex) {
	if (fdIndex < STD_FDS) {
		PCB *process = getProcess(getCurrentPid());
		return process->fds[fdIndex];
	} else
		return fdIndex;
}

int *getFds() {
	PCB *process = getProcess(getCurrentPid());
	return process->fds;
}

static processInfo *getProcessInfo(PCB *process) {
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
	info->isForeground = process->isForeground;
	info->status = process->status;

	return info;
}