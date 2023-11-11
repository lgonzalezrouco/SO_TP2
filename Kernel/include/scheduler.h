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

void startShell(int16_t pid);

void addProcess(PCB *process);

PCB *getProcess(int16_t pid);

int16_t getCurrentPid();

void *schedule(void *currentSP);

int toggleBlockProcess(int16_t pid);

int setPriority(int16_t pid, uint8_t newPriority);

int getProcessesQty();

int killCurrentProcess(int retValue);

int killProcess(int16_t pid, int retValue);

void yield();

int waitpid(int16_t pid);

int16_t getNextPid();

int blockProcess(int16_t pid);

int unblockProcess(int16_t pid);

int16_t getFileDescriptor(uint8_t index);

#endif