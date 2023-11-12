#include <color.h>
#include <sh.h>
#include <shellPrograms.h>
#include <stdbool.h>
#include <syscalls.h>
#include <uStdio.h>
#include <uStdlib.h>
#include <uString.h>

#define toLower(n) ((n) >= 'A' && (n) <= 'Z' ? (n) - ('A' - 'a') : (n))
#define isValid(c) ((c) != ' ' && (c) != '|' && (c) != '&' && (c) != 0 && (c) != '\n')

#define QTY_BYTES         32 /* Cantidad de bytes de respuesta del printmem */
#define DEFAULT_FONT_SIZE 1
#define MIN_FONT_SIZE     1
#define MAX_FONT_SIZE     3

#define WELCOME         "\n\nBienvenido a bemaluOS!\n"
#define INVALID_COMMAND "Comando invalido!\n"
#define WRONG_PARAMS    "La cantidad de parametros ingresada es invalida\n"
#define TAB             "\t\t\t\t"

typedef struct Command {
	char *name;
	char *description;
	bool isBuiltin;
	ProcessCode code;
} Command;

static void analizeInput(char *input);
static void runProcess(char *input, int len);

/* BUILTIN FUNCTIONS */
static void help(int argc, char **args);
static int div(int argc, char **args);
static void printMem(int argc, char **args);
static void infoReg(int argc, char **args);

const static Command commands[] = {
    {"help", "Listado de comandos", true, (ProcessCode) help},
    {"inforeg", "Muestra la informacion de los registros", true, (ProcessCode) infoReg},
    {"time", "Despliega la hora actual UTC -3", false, (ProcessCode) time},
    {"div", "Hace la division entera de dos numeros naturales enviados por parametro", true, (ProcessCode) div},
    {"kaboom", "Ejecuta una excepcion de Invalid Opcode", true, (ProcessCode) kaboom},
    {"printmem", "Muestra el contenido de 32 bytes a partir de la direccion", true, (ProcessCode) printMem},
    {"clear", "Limpia toda la pantalla", true, (ProcessCode) clear},
    {"meminfo", "Imprime informacion de la memoria", false, (ProcessCode) printMemInfo},
    {"ps", "Imprime la lista de todos los procesos con sus propiedades", false, (ProcessCode) printProcesses},
    {"kill", "Mata un proceso", false, (ProcessCode) kill},
    {"nice", "Cambia la prioridad de un proceso", false, (ProcessCode) nice},
    {"block", "Bloquea o desbloquea el proceso del pid dado", false, (ProcessCode) toggleBlock},
    {"tm", "Corre el test de memoria, ingresar cantidad de memoria", false, (ProcessCode) test_mm},
    {"tp", "Corre el test de procesos, ingresar cantidad de procesos", false, (ProcessCode) test_processes},
    {"ts", "Corre el test de sincronizacion, ", false, (ProcessCode) test_sync},
    {"tprio", "Corre el test de prioridades, ", false, (ProcessCode) test_prio},
    {"tpipe", "Corre el test de pipes.", false, (ProcessCode) testNamedPipes}};

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
		;
		/*
		crear dos procesos pipeados
		*/
	} else
		runProcess(input, len);
}

static void runProcess(char *input, int len) {
	char program[MAX_CHARS] = {0};
	char arg1[MAX_CHARS] = {0};
	char arg2[MAX_CHARS] = {0};
	char isForeground = 1;

	if (input[len - 1] == '&') {
		isForeground = 0;
		input[--len] = 0;
		if (input[len - 1] == ' ')
			input[--len] = 0;
	}

	int i = 0;
	for (i = 0; isValid(input[i]); i++)
		program[i] = input[i];

	if (input[i] == ' ') {
		i++;
		for (int j = 0; isValid(input[i]); i++, j++)
			arg1[j] = input[i];

		if (input[i] == ' ') {
			i++;
			for (int k = 0; isValid(input[i]); i++, k++)
				arg2[k] = input[i];

			if (input[i] != 0) {
				printErr(INVALID_COMMAND);
				return;
			}
		} else if (input[i] != 0) {
			printErr(INVALID_COMMAND);
			return;
		}
	} else if (input[i] != 0) {
		printErr(INVALID_COMMAND);
		return;
	}

	bool found = false;
	for (int i = 0; i < sizeof(commands) / sizeof(Command) && !found; i++) {
		if (strcmp(commands[i].name, program) == 0) {
			found = true;

			char *args[] = {program, (arg1[0] == 0) ? NULL : arg1, (arg2[0] == 0) ? NULL : arg2, NULL};
			int argc = 0;
			for (int i = 0; i < 4; i++) {
				if (args[i] != NULL)
					argc++;
			}

			if (commands[i].isBuiltin) {
				commands[i].code(argc, args);
				return;
			}

			// todo hacer que no sea necesaria la prioridad
			// todo mandar isForeground y modificar los FDS estandard
			int16_t pid = createProcess(getPid(), commands[i].code, args, program, 5);
			if (pid == -1) {
				printErr("No se pudo crear el proceso\n");
				return;
			}
			if (isForeground)
				waitpid(pid);
		}
	}
	if (!found)
		printErr(INVALID_COMMAND);
}

static void help(int argc, char **args) {
	if (argc != 1) {
		printErr(WRONG_PARAMS);
		return;
	}

	for (int i = 0; i < sizeof(commands) / sizeof(Command); i++)
		printfc(SILVER, "%s: %s\r\n", commands[i].name, commands[i].description);
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