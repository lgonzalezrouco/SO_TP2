#include <semaphores.h>
#include <stddef.h>
#include <types.h>

#define MAX_SEMAPHORES 1024

static Semaphore * getSemaphore(const char * name);
static int getNextSemaphoreID();
static void freeSemaphores(Semaphore * semaphore);

Semaphore * semaphores[MAX_SEMAPHORES];

void initializeSemaphores() {
	for (int i = 0; i < MAX_SEMAPHORES; i++)
		semaphores[i] = NULL;
}

int sem_init(const char * name, uint32_t value) {
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

	return 0;
}

int sem_open(const char * name) {
}

int sem_close(const char * name) {
	Semaphore * semaphore = getSemaphore(name);
	if (semaphore == NULL)
		return -1;

	freeSemaphore(semaphore);
	semaphores[semaphore->id] = NULL;
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

static void freeSemaphores(Semaphore * semaphore) {
	free(semaphore);
}