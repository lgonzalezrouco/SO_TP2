#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include "lib.h"
#include "processes.h"
#include "queueADT.h"
#include "types.h"

#define MAX_PROCESSES 128
#define IDLE_PID      0
#define SHELL_PID     1

void initializeScheduler();

void startShell(uint16_t pid);

void addProcess(PCB *process);

PCB *removeProcess(PCB *process);

PCB *getProcess(uint16_t pid);

uint16_t getCurrentPid();

int setPriority(uint16_t pid, uint8_t newPriority);

void *schedule(void *currentSP);

void stopProcess(void *currentSP);

int getProcessesQty();

int setStatus(uint16_t pid, processStatus newStatus);

void yield();

#endif