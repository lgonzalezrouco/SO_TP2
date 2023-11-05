#ifndef _PROCESSES_H_
#define _PROCESSES_H_

#include "memoryManager.h"
#include "lib.h"
#include "scheduler.h"
#include "types.h"
#include "interrupts.h"

#define STACK_SIZE 4096

void processWrapper(ProcessCode code, char **args);

int createProcess(uint16_t parentPid, ProcessCode code, char **args, char *name,
		  uint8_t priority);

int killProcess(uint16_t pid);

int idle(int argc, char **argv);

processInfo *getProcessInfo(PCB *process);

processInfo **getProcessesInfo();

#endif