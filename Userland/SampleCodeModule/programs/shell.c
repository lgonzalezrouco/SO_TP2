// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <color.h>
#include <shell.h>
#include <shellPrograms.h>
#include <stdbool.h>
#include <syscalls.h>
#include <uStdio.h>
#include <uStdlib.h>
#include <uString.h>

#define toLower(n) ((n) >= 'A' && (n) <= 'Z' ? (n) - ('A' - 'a') : (n))
#define isValid(c) ((c) != ' ' && (c) != '|' && (c) != '&' && (c) != 0 && (c) != '\n')

#define QTY_BYTES 32 /* Cantidad de bytes de respuesta del printmem */

#define WELCOME         "\n\nBienvenido a bemaluOS!\n"
#define INVALID_COMMAND "Comando invalido!\n"
#define WRONG_PARAMS    "La cantidad de parametros ingresada es invalida\n"
#define TAB             "\t\t\t\t"

typedef struct Command {
	char *name;
	char *description;
	ProcessCode code;
} Command;

static void analizeInput(char *input);
static int createSingleProcess(char *input, int len);
static int createPipedProcess(char *input, int len, int fds[]);

/* BUILTIN FUNCTIONS */
static void help(int argc, char **args);
static int div(int argc, char **args);
static void printMem(int argc, char **args);
static void infoReg(int argc, char **args);
static void writeSharedMemory(int argc, char **args);
static void readSharedMemory(int argc, char **args);

// todo buddy
// todo pvs

const static Command commands[] = {
    {"help", "Listado de comandos", (ProcessCode) help},
    {"inforeg", "Muestra la informacion de los registros", (ProcessCode) infoReg},
    {"time", "Despliega la hora actual UTC -3", (ProcessCode) time},
    {"div", "Hace la division entera de dos numeros naturales enviados por parametro", (ProcessCode) div},
    {"kaboom", "Ejecuta una excepcion de Invalid Opcode", (ProcessCode) kaboom},
    {"printmem", "Muestra el contenido de 32 bytes a partir de la direccion", (ProcessCode) printMem},
    {"clear", "Limpia toda la pantalla", (ProcessCode) clear},
    {"meminfo", "Imprime informacion de la memoria", (ProcessCode) printMemInfo},
    {"ps", "Imprime la lista de todos los procesos con sus propiedades", (ProcessCode) printProcesses},
    {"kill", "Mata un proceso", (ProcessCode) kill},
    {"nice", "Cambia la prioridad de un proceso", (ProcessCode) nice},
    {"block", "Bloquea o desbloquea el proceso del pid dado", (ProcessCode) toggleBlock},
    {"tm", "Corre el test de memoria, ingresar cantidad de memoria", (ProcessCode) test_mm},
    {"tp", "Corre el test de procesos, ingresar cantidad de procesos", (ProcessCode) test_processes},
    {"ts", "Corre el test de sincronizacion, ", (ProcessCode) test_sync},
    {"tprio", "Corre el test de prioridades, ", (ProcessCode) test_prio},
    {"wc", "Cuenta la cantidad de lineas del input", (ProcessCode) wc},
    {"cat", "Imprime el STDIN tal como lo recibe", (ProcessCode) cat},
    {"filter", "Filtra las vocales del input", (ProcessCode) filter},
    {"loop", "Imprime su ID con un saludo cada una determinada cantidad de segundos", (ProcessCode) loop},
    {"philo", "Corre el problema de los filosofos comensales", (ProcessCode) philo},
    {"yield", "Renunciar a la CPU", (ProcessCode) yield},
    {"write", "Escribo en la shared memory", (ProcessCode) writeSharedMemory},
    {"read", "Leo de la shared memory", (ProcessCode) readSharedMemory}};

void shell() {
	puts(WELCOME);
	while (1) {
		printfc(LIGHT_GREEN, "bemaluOS> ");
		char input[MAX_CHARS] = {0};
		scanf("%i", input);
		analizeInput(input);
	}
}

static void analizeInput(char *input) {
	char foundPipe = 0;
	int len;
	for (len = 0; input[len] != 0; len++) {
		if (input[len] == '|') {
			if (foundPipe) {
				printErr(INVALID_COMMAND);
				return;
			} else
				foundPipe = 1;
		}
		input[len] = toLower(input[len]);
	}
	if (foundPipe) {
		int pipeId = getNextPipeId();
		if (pipeId == NOT_FOUND) {
			printErr("No hay mas pipes disponibles\n");
			return;
		}
		int writeFds[] = {STDIN, pipeId, STDERR};
		int readFds[] = {pipeId, STDOUT, STDERR};

		char writeInput[MAX_CHARS] = {0};
		char readInput[MAX_CHARS] = {0};

		int writeLen = strcpychar(writeInput, input, '|');
		int readLen = strcpy(readInput, input[writeLen + 1] == ' ' ? input + writeLen + 2 : input + writeLen + 1);

		if (writeInput[writeLen - 1] == ' ')
			writeInput[--writeLen] = 0;

		int writePid = createPipedProcess(writeInput, writeLen, writeFds);
		if (writePid == -1)
			return;

		int readPid = createPipedProcess(readInput, readLen, readFds);
		if (readPid == -1) {
			killProcess(writePid);
			return;
		}

		waitpid(writePid);
		waitpid(readPid);
	} else
		createSingleProcess(input, len);
}

int createSingleProcess(char *input, int len) {
	char program[MAX_CHARS] = {0};
	char arg1[MAX_CHARS_ARG1] = {0};
	char arg2[MAX_CHARS] = {0};
	char isForeground = 1;

	if (len > 0 && input[len - 1] == '&') {
		isForeground = 0;
		input[--len] = 0;
		if (len > 0 && input[len - 1] == ' ')
			input[--len] = 0;
	}

	int programLen = strcpychar(program, input, ' ');
	int arg1Len = 0;

	if (input[programLen] != 0)
		arg1Len = strcpychar(arg1, input + programLen + 1, ' ');

	if (input[programLen + arg1Len + 1] != 0)
		strcpychar(arg2, input + programLen + arg1Len + 2, ' ');

	for (int i = 0; i < sizeof(commands) / sizeof(Command); i++) {
		if (strcmp(commands[i].name, program) == 0) {
			char *args[] = {program, (arg1[0] == 0) ? NULL : arg1, (arg2[0] == 0) ? NULL : arg2, NULL};

			int16_t pid = createProcess(commands[i].code, args, program, isForeground);
			if (pid == -1) {
				printErr("No se pudo crear el proceso\n");
				return -1;
			}
			if (isForeground)
				waitpid(pid);

			return pid;
		}
	}
	printErr(INVALID_COMMAND);
	return -1;
}

int createPipedProcess(char *input, int len, int fds[]) {
	char program[MAX_CHARS] = {0};
	char arg1[MAX_CHARS_ARG1] = {0};
	char arg2[MAX_CHARS] = {0};

	if (len > 0 && input[len - 1] == '&')
		return -1;

	int programLen = strcpychar(program, input, ' ');
	int arg1Len = 0;
	if (input[programLen] != 0)
		arg1Len = strcpychar(arg1, input + programLen + 1, ' ');
	if (input[programLen + arg1Len + 1] != 0)
		strcpychar(arg2, input + programLen + arg1Len + 2, ' ');

	for (int i = 0; i < sizeof(commands) / sizeof(Command); i++) {
		if (strcmp(commands[i].name, program) == 0) {
			char *args[] = {program, (arg1[0] == 0) ? NULL : arg1, (arg2[0] == 0) ? NULL : arg2, NULL};

			int16_t pid = createProcessFds(commands[i].code, args, program, true, fds);
			return pid;
		}
	}
	printErr(INVALID_COMMAND);
	return -1;
}

static void help(int argc, char **args) {
	if (argc != 1) {
		printErr(WRONG_PARAMS);
		return;
	}

	for (int i = 0; i < sizeof(commands) / sizeof(Command); i++)
		printf("%s: %s\r\n", commands[i].name, commands[i].description);
}

static int div(int argc, char **args) {
	if (argc != 3 || args[1] == NULL || args[2] == NULL) {
		printErr(WRONG_PARAMS);
		return 1;
	}
	printf("%s / %s = %d\r\n", args[1], args[2], atoi(args[1]) / atoi(args[2]));
	return 0;
}

static void printMem(int argc, char **args) {
	if (argc != 2) {
		printErr(WRONG_PARAMS);
		return;
	}

	uint8_t resp[QTY_BYTES];
	char *end;
	getMemory(strtoh(args[1], &end), resp);
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

static void infoReg(int argc, char **args) {
	if (argc != 1) {
		printErr(WRONG_PARAMS);
		return;
	}
	int len = sizeof(_regNames) / sizeof(char *);
	uint64_t regSnapshot[len];
	getInfoReg(regSnapshot);
	for (int i = 0; i < len; i++) {
		printf("%s: 0x%x\n", _regNames[i], regSnapshot[i]);
	}
}

static void writeSharedMemory(int argc, char **args) {
	if (argc != 1) {
		printErr(WRONG_PARAMS);
		return;
	}

	char *mem = (char *) openSharedMemory(0, 100);
	if (mem == NULL) {
		printErr("No se pudo abrir la memoria compartida\n");
		return;
	}

	char string[] = "BemaluOS";

	for (int i = 0; string[i]; i++)
		mem[i] = string[i];
}

static void readSharedMemory(int argc, char **args) {
	if (argc != 1) {
		printErr(WRONG_PARAMS);
		return;
	}

	char *mem = (char *) openSharedMemory(0, 100);
	if (mem == NULL) {
		printErr("No se pudo abrir la memoria compartida\n");
		return;
	}

	for (int i = 0; mem[i]; i++)
		printf("%c", mem[i]);
	printf("\n");
}