#include <semaphores.h>

#define MAX_SEMAPHORES 1024

static Semaphore * getSemaphore(const char * name);
static int getNextSemaphoreID();
static void freeSemaphore(Semaphore * semaphore);
static void getExclusiveAccess(Semaphore * semaphore);
static void releaseExclusiveAccess(Semaphore * semaphore);
static void resumeUnblockedProcess(QueueADT queue);

Semaphore * semaphores[MAX_SEMAPHORES];

void initializeSemaphores() {
	for (int i = 0; i < MAX_SEMAPHORES; i++)
		semaphores[i] = NULL;
}

int semInit(char * name, uint32_t value) {
	if (getSemaphore(name) != NULL)
		return -1;

	int id = getNextSemaphoreID();
	if (id == -1)
		return -1;

	semaphores[id] = (Semaphore *) malloc(sizeof(Semaphore));
	if (semaphores[id] == NULL)
		return -1;

	strcpy(semaphores[id]->name, name);
	semaphores[id]->id = id;
	semaphores[id]->value = value;
	semaphores[id]->mutex = 0;
	semaphores[id]->blockedProcesses = createQueueADT();
	semaphores[id]->mutexProcesses = createQueueADT();

	return 0;
}

int semOpen(const char * name) {
	Semaphore * semaphore = getSemaphore(name);
	if (semaphore == NULL)
		return -1;

	return semaphore->id;
}

int semClose(const char * name) {
	Semaphore * semaphore = getSemaphore(name);
	if (semaphore == NULL)
		return -1;

	freeSemaphore(semaphore);
	semaphores[semaphore->id] = NULL;
	return 0;
}

int semWait(const char * name) {
	Semaphore * semaphore = getSemaphore(name);
	if (semaphore == NULL)
		return -1;

	getExclusiveAccess(semaphore);

	while (semaphore->value == 0) {
		uint16_t pid = getCurrentPid();
		PCB * process = getProcess(pid);
		if(process == NULL)
			return -1;
		enqueue(semaphore->blockedProcesses, process);
		blockProcess(pid);
		releaseExclusiveAccess(semaphore);
		yield();
		getExclusiveAccess(semaphore);
	}
	semaphore->value--;

	releaseExclusiveAccess(semaphore);
	return 0;
}

int semPost(const char * name) {
	Semaphore * semaphore = getSemaphore(name);
	if (semaphore == NULL)
		return -1;

	getExclusiveAccess(semaphore);

	semaphore->value++;
	if (semaphore->value == 0) {
		releaseExclusiveAccess(semaphore);
		return -1;
	}
	resumeUnblockedProcess(semaphore->blockedProcesses);

	releaseExclusiveAccess(semaphore);
	return 0;
}

static Semaphore * getSemaphore(const char * name) {
	for (int i = 0; i < MAX_SEMAPHORES; i++) {
		if (semaphores[i] != NULL && strcmp(semaphores[i]->name, name) == 0)
			return semaphores[i];
	}
	return NULL;
}

static int getNextSemaphoreID() {
	for (int i = 0; i < MAX_SEMAPHORES; i++) {
		if (semaphores[i] == NULL)
			return i;
	}
	return -1;
}

static void freeSemaphore(Semaphore * semaphore) {
	freeQueue(semaphore->mutexProcesses);
	freeQueue(semaphore->blockedProcesses);
	free(semaphore);
}

static void getExclusiveAccess(Semaphore * semaphore) {
	while (criticalRegion(&(semaphore->mutex))) {
		uint16_t pid = getCurrentPid();
		PCB * process = getProcess(pid);
		if(process == NULL)
			return;
		enqueue(semaphore->mutexProcesses, process);
		blockProcess(pid);
		yield();
	}
}

static void releaseExclusiveAccess(Semaphore * semaphore) {
	resumeUnblockedProcess(semaphore->mutexProcesses);
	semaphore->mutex = 0;
}

static void resumeUnblockedProcess(QueueADT queue) {
	if (!isEmpty(queue)) {
		PCB * process = dequeue(queue);
		if (process != NULL){
			unblockProcess(process->pid);
		}
	}
}
