#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include "queueADT.h"
#include "types.h"

#define PRIORITY_LEVELS 7

void initializeScheduler();

void includeTerminal(uint16_t pid);

void addProcess(PCB *process);

PCB *removeProcess(PCB *process);

PCB *getProcess(uint16_t pid);

uint16_t getCurrentPid();

int setPriority(uint16_t pid, int newPriority);

void *schedule(void *currentSP);

#endif