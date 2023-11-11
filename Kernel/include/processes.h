#ifndef _PROCESSES_H_
#define _PROCESSES_H_

#include "interrupts.h"
#include "lib.h"
#include "memoryManager.h"
#include "pipes.h"
#include "scheduler.h"
#include "types.h"

#define STACK_SIZE 4096

void resetPIDCounter();

void processWrapper(ProcessCode code, char **args);

int createProcess(int16_t parentPid, ProcessCode code, char **args, char *name, uint8_t priority, int fds[]);

int idle(int argc, char **argv);

void freeProcess(PCB *process);

PCB **getProcessesInfo();

void freeProcessesInfo(PCB **infoArray);

uint16_t getFdValue(uint8_t fdIndex);

#endif