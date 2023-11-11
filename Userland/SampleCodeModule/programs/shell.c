// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <libasm.h>
#include <man.h>
#include <shell.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syscalls.h>
#include <test_mm.h>
#include <test_prio.h>
#include <test_processes.h>
#include <test_sync.h>
#include <test_pipes.h>
#include <test_util.h>
#include <types.h>

/* Enum para la cantidad de argumentos recibidos */
typedef enum { NO_PARAMS = 0, SINGLE_PARAM, DUAL_PARAM } functionType;
#define QTY_BYTES         32 /* Cantidad de bytes de respuesta del printmem */
#define DEFAULT_FONT_SIZE 1
#define MIN_FONT_SIZE     1
#define MAX_FONT_SIZE     3

#define WELCOME           "\n\nBienvenido a bemaluOS!\n"
#define INVALID_COMMAND   "Comando invalido!\n"
#define WRONG_PARAMS      "La cantidad de parametros ingresada es invalida\n"
#define INVALID_FONT_SIZE "Dimension invalida de fuente\n"
#define CHECK_MAN         "Escriba \"man %s\" para ver como funciona el comando\n"
#define CHECK_MAN_FONT    "Escriba \"man font-size\" para ver las dimensiones validas\n"

typedef struct {
	char *name;         // Nombre del comando
	char *description;  // Descripcion del comando (para help)
	union {             // Puntero a la funcion
		int (*f)(void);

		int (*g)(char *);

		int (*h)(char *, char *);
	};

	functionType ftype;  // Cantidad de argumentos del comando
} Command;

static void help();
static void printHelp();

static void man(char *command);

static void printInfoReg();

static void time();

static int div(char *num, char *div);

static void fontSize(char *size);

static void printMem(char *pos);

static int getCommandIndex(char *command);

static void printMemInfo();

static void printProcesses();

static void testProcesses();

static void kill(char *pid);

static void nice(char *pid, char *priority);

static void ps();

static void toggleBlock(char *pid);

static Command commands[] = {
    {"help", "Listado de comandos", .f = (void *) &help, NO_PARAMS},
    {"man", "Manual de uso de los comandos", .g = (void *) &man, SINGLE_PARAM},
    {"inforeg",
     "Informacion de los registros que fueron caputrados en un momento arbitrario de ejecucion del sistema",
     .f = (void *) &printInfoReg,
     NO_PARAMS},
    {"time", "Despliega la hora actual UTC -3", .f = (void *) &time, NO_PARAMS},
    {"div", "Hace la division entera de dos numeros naturales enviados por parametro", .h = (void *) &div, DUAL_PARAM},
    {"kaboom", "Ejecuta una excepcion de Invalid Opcode", .f = (void *) &kaboom, NO_PARAMS},
    {"font-size",
     "Cambio de dimensiones de la fuente. Para hacerlo escribir el comando seguido de un numero",
     .g = (void *) &fontSize,
     SINGLE_PARAM},
    {"printmem",
     "Realiza un vuelco de memoria de los 32 bytes posteriores a una direccion de memoria en "
     "formato hexadecimal enviada por parametro",
     .g = (void *) &printMem,
     SINGLE_PARAM},
    {"clear", "Limpia toda la pantalla", .f = (void *) &clear, NO_PARAMS},
    {"meminfo", "Imprime informacion de la memoria", .f = (void *) &printMemInfo, NO_PARAMS},
    {"ps", "Imprime la lista de todos los procesos con sus propiedades", .f = (void *) &ps, NO_PARAMS},
    {"test-proc", "Testea la creacion de procesos", .f = (void *) &testProcesses, NO_PARAMS},
    {"kill", "Mata un proceso", .g = (void *) &kill, SINGLE_PARAM},
    {"nice", "Cambia la prioridad de un proceso", .h = (void *) &nice, DUAL_PARAM},
    {"tm", "Corre el test de memoria, ingresar cantidad de memoria", .g = (void *) &test_mm, SINGLE_PARAM},
    {"tp", "Corre el test de procesos, ingresar cantidad de procesos", .g = (void *) &test_processes, SINGLE_PARAM},
    {"block", "Bloquea o desbloquea el proceso del pid dado", .g = (void *) &toggleBlock, SINGLE_PARAM},
    {"ts", "Corre el test de sincronizacion, ", .h = (void *) &test_sync, DUAL_PARAM},
    {"tprio", "Corre el test de prioridades, ", .f = (void *) &test_prio, NO_PARAMS},
	{"tpipe", "Corre el test de pipes.", .f = (void *) &testNamedPipes, NO_PARAMS},
};

void run_shell() {
	int index;
	puts(WELCOME);
	while (1) {
		putchar('>');
		char command[MAX_CHARS] = {0};
		char arg1[MAX_CHARS];
		char arg2[MAX_CHARS];
		int qtyParams = scanf("%s %s %s", command, arg1, arg2);
		index = getCommandIndex(command);
		if (index == -1) {
			if (command[0] != 0)
				printErr(INVALID_COMMAND);
			continue;
		}
		int funcParams = commands[index].ftype;
		if (qtyParams - 1 != funcParams) {
			printErr(WRONG_PARAMS);
			printf(CHECK_MAN, command);
			continue;
		}
		switch (commands[index].ftype) {
			case NO_PARAMS:
				commands[index].f();
				break;
			case SINGLE_PARAM:
				commands[index].g(arg1);
				break;
			case DUAL_PARAM:
				commands[index].h(arg1, arg2);
				break;
		}
	}
}

static int getCommandIndex(char *command) {
	int idx = 0;
	for (; idx < QTY_COMMANDS; idx++) {
		if (!strcmp(commands[idx].name, command))
			return idx;
	}
	return -1;
}

static void printHelp() {
	for (int i = 0; i < QTY_COMMANDS; i++)
		printf("%s: %s\r\n", commands[i].name, commands[i].description);
}

static int div(char *num, char *div) {
	printf("%s/%s=%d\r\n", num, div, atoi(num) / atoi(div));
	return 1;
}

static void time() {
	uint32_t secs = getSeconds();
	uint32_t h = secs / 3600, m = secs % 3600 / 60, s = secs % 3600 % 60;
	printf("%2d:%2d:%2d\r\n", h, m, s);
}

static void fontSize(char *size) {
	int s = atoi(size);
	if (s >= MIN_FONT_SIZE && s <= MAX_FONT_SIZE)
		setFontSize((uint8_t) atoi(size));
	else {
		printErr(INVALID_FONT_SIZE);
		puts(CHECK_MAN_FONT);
	}
}

static void printMem(char *pos) {
	uint8_t resp[QTY_BYTES];
	char *end;
	getMemory(strtoh(pos, &end), resp);
	for (int i = 0; i < QTY_BYTES; i++) {
		printf("0x%2x ", resp[i]);
		if (i % 4 == 3)
			putchar('\n');
	}
}

static char *_regNames[] = {"RIP",
                            "RSP",
                            "RAX",
                            "RBX",
                            "RCX",
                            "RDX",
                            "RBP",
                            "RDI",
                            "RSI",
                            "R8",
                            "R9",
                            "R10",
                            "R11",
                            "R12",
                            "R13",
                            "R14",
                            "R15"};

static void printInfoReg() {
	int len = sizeof(_regNames) / sizeof(char *);
	uint64_t regSnapshot[len];
	getInfoReg(regSnapshot);
	for (int i = 0; i < len; i++)
		printf("%s: 0x%x\n", _regNames[i], regSnapshot[i]);
}

static void man(char *command) {
	int idx = getCommandIndex(command);
	if (idx != -1)
		printf("%s\n", usages[idx]);
	else
		printErr(INVALID_COMMAND);
}

static void printMemInfo() {
	memoryInfo *info = getMemoryInfo();

	printf("Total memory: %d B\n", info->totalMemory);
	printf("Used memory: %d B\n", info->usedMemory);
	printf("Free memory: %d B\n", info->freeMemory);
}

static void ps() {
	char *psArgs[] = {"ps", NULL};
	int pid = createProcess((int16_t) 1, (ProcessCode) &printProcesses, psArgs, "ps", (uint8_t) 5);
	if (pid != -1)
		waitpid((int16_t) pid);
}

static void help() {
	char *helpArgs[] = {"help", NULL};
	int pid = createProcess((int16_t) 1, (ProcessCode) &printHelp, helpArgs, "help", (uint8_t) 6);
	if (pid != -1)
		waitpid((int16_t) pid);
}

static void printProcesses() {
	PCB **info = getProcessesInfo();
	int i = 0;

	// Encabezados de la tabla
	printf("PID\t\t\tPARENT PID\t\t\tPRIORITY\t\t\tSTATUS\t\t\tSTACK BASE\t\t\tSTACK POINTER\t\t\tNAME\n");

	while (info[i] != NULL) {
		printf("%d\t\t\t", info[i]->pid);
		if (info[i]->pid < 100)
			printf("\t");
		if (info[i]->pid < 10)
			printf("\t");
		printf("%d\t\t\t\t\t\t\t\t\t\t", info[i]->parentPid);
		if (info[i]->parentPid < 100)
			printf("\t");
		if (info[i]->parentPid < 10)
			printf("\t");
		if (info[i]->priority == 6)
			printf("BLOCKED\t\t\t\t");
		else if (info[i]->priority == 0)
			printf("IDLE\t\t\t\t\t\t\t");
		else
			printf("%d\t\t\t\t\t\t\t\t\t\t", info[i]->priority);
		switch (info[i]->status) {
			case 0:
				printf("RUNNING\t\t");
				break;
			case 1:
				printf("BLOCKED\t\t");
				break;
			case 3:
				printf("READY\t\t\t\t");
				break;
			default:
				printf("UNKNOWN\t\t");
				break;
		}
		printf("0x%x\t\t\t\t\t", info[i]->stack->base);
		printf("0x%x\t\t\t\t\t\t\t\t", info[i]->stack->current);
		printf("%s", info[i]->name);
		putchar('\n');
		i++;
	}
	freeProcessesInfo(info);
}

static void testProcesses() {
	char *helpArgs[] = {"loop", NULL};
	createProcess((int16_t) 1, (ProcessCode) &endless_loop, helpArgs, "endless_loop", (uint8_t) 6);
	// char * helpProp[] = {"5"};
	// int pid = createProcess((int16_t) 1, (ProcessCode) &test_processes, helpProp, "test_processes", (uint8_t) 6);
}

static void kill(char *pid) {
	int result = killProcess((int16_t) atoi(pid));
	if (result == NOT_FOUND) {
		printErr("Es inexistente el proceso con pid ");
		printf("%s\n", pid);
	} else if (result == INVALID_PROCESS) {
		printErr("No se puede matar al proceso con pid ");
		printf("%s\n", pid);
	} else
		printf("Se mato al proceso con pid %s\n", pid);
}

static void nice(char *pid, char *priority) {
	int result = setPriority((int16_t) atoi(pid), (uint8_t) atoi(priority));
	if (result == NOT_FOUND) {
		printErr("Es inexistente el proceso con pid ");
		printf("%s\n", pid);
	} else if (result == INVALID_PROCESS) {
		printErr("No se puede cambiar la prioridad al proceso con pid ");
		printf("%s\n", pid);
	} else if (result == INVALID_PRIORITY)
		printErr("La prioridad ingresada es invalida, debe ser entre 0 y 6\n");
	else if (result == SAME_PRIORITY)
		printErr("La prioridad ingresada es la misma que la actual\n");
	else
		printf("Se cambio la prioridad del proceso %s a %s\n", pid, priority);
}

static void toggleBlock(char *pid) {
	int result = toggleBlockProcess((int16_t) atoi(pid));
	if (result == NOT_FOUND) {
		printErr("Es inexistente el proceso con pid ");
		printf("%s\n", pid);
	} else if (result == INVALID_PROCESS) {
		printErr("No se puede bloquear al proceso con pid ");
		printf("%s\n", pid);
	} else
		printf("Se bloqueo al proceso con pid %s\n", pid);
}
