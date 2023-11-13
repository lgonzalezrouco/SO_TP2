// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <color.h>
#include <keyboard.h>
#include <lib.h>
#include <memoryManager.h>
#include <processes.h>
#include <registers.h>
#include <semaphores.h>
#include <sharedMemory.h>
#include <speaker.h>
#include <stdint.h>
#include <time.h>
#include <video.h>

/* File Descriptors*/
#define STDIN  0
#define STDOUT 1
#define STDERR 2
#define KBDIN  3

static int64_t syscall_read(int16_t fd, char *buffer, uint64_t count);
static void syscall_write(int32_t fd, char c);
static void syscall_clear();
static uint32_t syscall_seconds();
static uint64_t *syscall_registerArray(uint64_t *regarr);
static void syscall_fontSize(uint8_t size);
static uint32_t syscall_resolution();
static void syscall_drawRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);
static uint64_t syscall_getTicks();
static void syscall_getMemory(uint64_t pos, uint8_t *vec);
static void syscall_playSound(uint64_t frequency, uint64_t ticks);
static void syscall_setFontColor(uint8_t r, uint8_t g, uint8_t b);
static uint32_t syscall_getFontColor();
static uint64_t syscall_malloc(uint64_t size);
static void syscall_free(uint64_t ptr);
static uint64_t syscall_getMemoryInfo();
static processInfo **syscall_getProcessesInfo();
static void syscall_freeProcessesInfo(uint64_t infoArray);
static uint64_t syscall_createProcess(ProcessCode code, char **args, char *name, uint8_t isForeground, int fds[]);
static uint64_t syscall_killProcess(int16_t pid);
static uint64_t syscall_setPriority(int16_t pid, uint8_t priority);
static uint64_t syscall_waitpid(int16_t pid);
static uint64_t syscall_toggleBlock(int16_t pid);
static uint64_t syscall_getPid();
static uint64_t syscall_yield();
static uint64_t syscall_semInit(char *name, uint32_t value);
static uint64_t syscall_semOpen(char *name);
static uint64_t syscall_semClose(char *name);
static uint64_t syscall_semWait(char *name);
static uint64_t syscall_semPost(char *name);
static uint64_t syscall_openPipe(uint16_t id, uint8_t mode, uint16_t pid);
static uint64_t syscall_closePipe(uint16_t id, uint16_t pid);
static uint64_t syscall_unblockProcess(int16_t pid);
static void syscall_sleep(int seconds);
static int syscall_getNextPipeId();
static int *syscall_getFds();
static void *openSharedMem(int id, int size);

typedef uint64_t (*sysFunctions)(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t);

static sysFunctions sysfunctions[] = {
    (sysFunctions) syscall_read,
    (sysFunctions) syscall_write,
    (sysFunctions) syscall_clear,
    (sysFunctions) syscall_seconds,
    (sysFunctions) syscall_registerArray,
    (sysFunctions) syscall_fontSize,
    (sysFunctions) syscall_resolution,
    (sysFunctions) syscall_drawRect,
    (sysFunctions) syscall_getTicks,
    (sysFunctions) syscall_getMemory,
    (sysFunctions) syscall_playSound,
    (sysFunctions) syscall_setFontColor,
    (sysFunctions) syscall_getFontColor,
    (sysFunctions) syscall_malloc,
    (sysFunctions) syscall_free,
    (sysFunctions) syscall_getMemoryInfo,
    (sysFunctions) syscall_getProcessesInfo,
    (sysFunctions) syscall_freeProcessesInfo,
    (sysFunctions) syscall_createProcess,
    (sysFunctions) syscall_killProcess,
    (sysFunctions) syscall_setPriority,
    (sysFunctions) syscall_waitpid,
    (sysFunctions) syscall_toggleBlock,
    (sysFunctions) syscall_getPid,
    (sysFunctions) syscall_yield,
    (sysFunctions) syscall_semInit,
    (sysFunctions) syscall_semOpen,
    (sysFunctions) syscall_semClose,
    (sysFunctions) syscall_semWait,
    (sysFunctions) syscall_semPost,
    (sysFunctions) syscall_openPipe,
    (sysFunctions) syscall_closePipe,
    (sysFunctions) syscall_unblockProcess,
    (sysFunctions) syscall_sleep,
    (sysFunctions) syscall_getNextPipeId,
    (sysFunctions) syscall_getFds,
    (sysFunctions) openSharedMem,
};

uint64_t syscallDispatcher(
    uint64_t id, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5) {
	return sysfunctions[id](arg0, arg1, arg2, arg3, arg4, arg5);
}

// Read char
static int64_t syscall_read(int16_t fd, char *buffer, uint64_t count) {
	if (fd == DEV_NULL) {
		buffer[0] = EOF;
		return 0;
	}

	uint16_t fdValue = getFdValue(fd);
	if (fdValue == STDIN) {
		for (uint64_t i = 0; i < count; i++) {
			buffer[i] = getAscii();
			if ((int) buffer[i] == EOF)
				return i + 1;
		}
		return count;
	} else if (fdValue >= STD_FDS)
		return readPipe(fdValue, buffer, count, getCurrentPid());
	return -1;
}

// Write char
static void syscall_write(int32_t fd, char c) {
	if (fd <= DEV_NULL)
		return;

	uint16_t fdValue = getFdValue(fd);
	if (fdValue == STDOUT || fdValue == STDERR) {
		Color prevColor = getFontColor();
		if (fdValue == STDERR)
			setFontColor(ERROR_COLOR);
		printChar(c);
		setFontColor(prevColor);
	} else if (fdValue >= STD_FDS)
		writePipe(fdValue, &c, 1, getCurrentPid());
}

// Clear
static void syscall_clear() {
	videoClear();
}

// Get time in seconds
static uint32_t syscall_seconds() {
	return getSeconds();
}

// Get register snapshot array
static uint64_t *syscall_registerArray(uint64_t *regarr) {
	uint64_t *snapshot = getLastRegSnapshot();
	for (int i = 0; i < QTY_REGS; i++)
		regarr[i] = snapshot[i];
	return regarr;
}

// Set fontsize
static void syscall_fontSize(uint8_t size) {
	setFontSize(size - 1);
}

// Get screen resolution
static uint32_t syscall_resolution() {
	return getScreenResolution();
}

// DrawRect
static void syscall_drawRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
	ColorInt myColor = {bits: color};
	drawRect(x, y, width, height, myColor.color);
}

// GetTicks
static uint64_t syscall_getTicks() {
	return ticksElapsed();
}

// PrintMem
static void syscall_getMemory(uint64_t pos, uint8_t *vec) {
	memcpy(vec, (uint8_t *) pos, 32);
}

// playSound
static void syscall_playSound(uint64_t frequency, uint64_t ticks) {
	playSound(frequency, ticks);
}

// Set fontsize
static void syscall_setFontColor(uint8_t r, uint8_t g, uint8_t b) {
	setFontColor((Color){b, g, r});
}

// Get fontsize
static uint32_t syscall_getFontColor() {
	ColorInt c = {color: getFontColor()};
	return c.bits;
}

static uint64_t syscall_malloc(uint64_t size) {
	return (uint64_t) malloc(size);
}

static void syscall_free(uint64_t ptr) {
	free((void *) ptr);
}

static uint64_t syscall_getMemoryInfo() {
	return (uint64_t) getMemoryInfo();
}

static processInfo **syscall_getProcessesInfo() {
	return (processInfo **) getProcessesInfo();
}

static void syscall_freeProcessesInfo(uint64_t infoArray) {
	freeProcessesInfo((processInfo **) infoArray);
}

static uint64_t syscall_createProcess(ProcessCode code, char **args, char *name, uint8_t isForeground, int fds[]) {
	return (uint64_t) createProcess(code, args, name, isForeground, fds);
}

static uint64_t syscall_killProcess(int16_t pid) {
	return (uint64_t) killProcess(pid, -1);
}

static uint64_t syscall_setPriority(int16_t pid, uint8_t priority) {
	return (uint64_t) setPriority(pid, priority);
}

static uint64_t syscall_waitpid(int16_t pid) {
	return (uint64_t) waitpid(pid);
}

static uint64_t syscall_toggleBlock(int16_t pid) {
	// si esta bloqueado lo desbloquea, si no lo bloquea
	return (uint64_t) toggleBlockProcess(pid);
}

static uint64_t syscall_getPid() {
	return (uint64_t) getCurrentPid();
}

static uint64_t syscall_yield() {
	yield();
	return 0;
}

static uint64_t syscall_semInit(char *name, uint32_t value) {
	return (uint64_t) semInit(name, value);
}

static uint64_t syscall_semOpen(char *name) {
	return (uint64_t) semOpen(name);
}

static uint64_t syscall_semClose(char *name) {
	return (uint64_t) semClose(name);
}

static uint64_t syscall_semWait(char *name) {
	return (uint64_t) semWait(name);
}

static uint64_t syscall_semPost(char *name) {
	return (uint64_t) semPost(name);
}

static uint64_t syscall_openPipe(uint16_t id, uint8_t mode, uint16_t pid) {
	return (uint64_t) openPipe(id, mode, pid);
}

static uint64_t syscall_closePipe(uint16_t id, uint16_t pid) {
	return (uint64_t) closePipe(id, pid);
}

static uint64_t syscall_unblockProcess(int16_t pid) {
	return (uint64_t) unblockProcess(pid);
}

static void syscall_sleep(int seconds) {
	sleep(seconds);
}

static int syscall_getNextPipeId() {
	return getNextPipeId();
}

static int *syscall_getFds() {
	return getFds();
}

static void *openSharedMem(int id, int size) {
	return (void *) openSharedMemory(id, size);
}
