#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include <processes.h>
#include <queueADT.h>

#define PRIORITY_LEVELS 7

void initializeScheduler();

int createProcess(int (*func)(int, char **), char **args, char *name,
		  uint8_t priority);

int setPriority(uint16_t pid, int newPriority);

#endif