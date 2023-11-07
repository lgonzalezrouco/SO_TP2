#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include "lib.h"
#include "processes.h"
#include "queueADT.h"
#include "types.h"

#define MAX_PROCESSES 128
#define MAX_QUANTUM   64
#define IDLE_PID      0
#define SHELL_PID     1

void initializeScheduler();

void startShell(uint16_t pid);

void addProcess(PCB *process);

PCB *getProcess(uint16_t pid);

uint16_t getCurrentPid();

void *schedule(void *currentSP);

void blockProcess(uint16_t pid);

void unblockProcess(uint16_t pid);

int setPriority(uint16_t pid, uint8_t newPriority);

int getProcessesQty();

int killCurrentProcess(int retValue);

int killProcess(uint16_t pid, int retValue);

void yield();

int waitpid(uint16_t pid);

#endif