// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <interrupts.h>
#include <keyboard.h>
#include <lib.h>
#include <memoryManager.h>
#include <moduleLoader.h>
#include <pipes.h>
#include <processes.h>
#include <scheduler.h>
#include <semaphores.h>
#include <stdint.h>
#include <sharedMemory.h>

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;

char *shellArgs[] = {"shell", NULL};
char *idleArgs[] = {"idle", NULL};

static void *const sampleCodeModuleAddress = (void *) 0x400000;
static void *const sampleDataModuleAddress = (void *) 0x500000;

typedef int (*EntryPoint)();

void clearBSS(void *bssAddress, uint64_t bssSize) {
	memset(bssAddress, 0, bssSize);
}

void *getStackBase() {
	return (void *) ((uint64_t) &endOfKernel + PageSize * 8  // The size of the stack itself, 32KiB
	                 - sizeof(uint64_t)                      // Begin at the top of the stack
	);
}

void initializeKernelBinary() {
	void *moduleAddresses[] = {sampleCodeModuleAddress, sampleDataModuleAddress};
	loadModules(&endOfKernelBinary, moduleAddresses);
	clearBSS(&bss, &endOfKernel - &bss);
}

int main() {
	load_idt();

	initializeMemoryManager();
	initializeScheduler();
	initializeSemaphores();
	initializeKeyboardDriver();
	initializePipes();
	initializeSharedMemory();

	int fdsIdle[] = {DEV_NULL, DEV_NULL, STDERR};
	int fdsShell[] = {STDIN, STDOUT, STDERR};

	createProcess((ProcessCode) &idle, idleArgs, "idle", 0, fdsIdle);
	int shellPid = createProcess((ProcessCode) sampleCodeModuleAddress, shellArgs, "shell", 1, fdsShell);

	startShell(shellPid);
	return 0;
}
