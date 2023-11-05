#include <libasm.h>
#include <man.h>
#include <shell.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syscalls.h>

/* Enum para la cantidad de argumentos recibidos */
typedef enum { NO_PARAMS = 0, SINGLE_PARAM, DUAL_PARAM } functionType;
#define QTY_BYTES	  32 /* Cantidad de bytes de respuesta del printmem */
#define DEFAULT_FONT_SIZE 1
#define MIN_FONT_SIZE	  1
#define MAX_FONT_SIZE	  3

#define WELCOME		  "\n\nBienvenido a bemaluOS!\n"
#define INVALID_COMMAND	  "Comando invalido!\n"
#define WRONG_PARAMS	  "La cantidad de parametros ingresada es invalida\n"
#define INVALID_FONT_SIZE "Dimension invalida de fuente\n"
#define CHECK_MAN	  "Escriba \"man %s\" para ver como funciona el comando\n"
#define CHECK_MAN_FONT                                                         \
  "Escriba \"man font-size\" para ver las dimensiones validas\n"

typedef struct {
  char *name;	      // Nombre del comando
  char *description;  // Descripcion del comando (para help)
  union {	      // Puntero a la funcion
    int (*f)(void);

    int (*g)(char *);

    int (*h)(char *, char *);
  };

  functionType ftype;  // Cantidad de argumentos del comando
} Command;

static void help();

static void man(char *command);

static void printInfoReg();

static void time();

static int div(char *num, char *div);

static void fontSize(char *size);

static void printMem(char *pos);

static int getCommandIndex(char *command);

static int getPrefixForMemory(int size);

static void printMemInfo();

static void ps();

static void testProcesses();

static void kill(char *pid);

static Command commands[] = {
    {"help", "Listado de comandos", .f = (void *)&help, NO_PARAMS},
    {"man", "Manual de uso de los comandos", .g = (void *)&man, SINGLE_PARAM},
    {"inforeg",
     "Informacion de los registros que fueron caputrados en un momento arbitrario de ejecucion del sistema",
     .f = (void *)&printInfoReg, NO_PARAMS},
    {"time", "Despliega la hora actual UTC -3", .f = (void *)&time, NO_PARAMS},
    {"div",
     "Hace la division entera de dos numeros naturales enviados por parametro",
     .h = (void *)&div, DUAL_PARAM},
    {"kaboom", "Ejecuta una excepcion de Invalid Opcode", .f = (void *)&kaboom,
     NO_PARAMS},
    {"font-size",
     "Cambio de dimensiones de la fuente. Para hacerlo escribir el comando seguido de un numero",
     .g = (void *)&fontSize, SINGLE_PARAM},
    {"printmem",
     "Realiza un vuelco de memoria de los 32 bytes posteriores a una direccion de memoria en "
     "formato hexadecimal enviada por parametro",
     .g = (void *)&printMem, SINGLE_PARAM},
    {"clear", "Limpia toda la pantalla", .f = (void *)&clear, NO_PARAMS},
    {"meminfo", "Imprime informacion de la memoria", .f = (void *)&printMemInfo,
     NO_PARAMS},
    {"ps", "Imprime la lista de todos los procesos con sus propiedades",
     .f = (void *)&ps, NO_PARAMS},
    {"test-proc", "Testea la creación de procesos", .f = (void *)&testProcesses,
     NO_PARAMS},
    {"kill", "Mata un proceso", .g = (void *)&kill, SINGLE_PARAM}};

void run_shell() {
  int index;
  puts(WELCOME);
  while (1) {
    putchar('>');
    char command[MAX_CHARS] = {0};
    char arg1[MAX_CHARS];
    char arg2[MAX_CHARS];
    int	 qtyParams = scanf("%s %s %s", command, arg1, arg2);
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
      case NO_PARAMS: commands[index].f(); break;
      case SINGLE_PARAM: commands[index].g(arg1); break;
      case DUAL_PARAM: commands[index].h(arg1, arg2); break;
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

static void help() {
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
    setFontSize((uint8_t)atoi(size));
  else {
    printErr(INVALID_FONT_SIZE);
    puts(CHECK_MAN_FONT);
  }
}

static void printMem(char *pos) {
  uint8_t resp[QTY_BYTES];
  char	 *end;
  getMemory(strtoh(pos, &end), resp);
  for (int i = 0; i < QTY_BYTES; i++) {
    printf("0x%2x ", resp[i]);
    if (i % 4 == 3)
      putchar('\n');
  }
}

static char *_regNames[] = {"RIP", "RSP", "RAX", "RBX", "RCX", "RDX",
			    "RBP", "RDI", "RSI", "R8",	"R9",  "R10",
			    "R11", "R12", "R13", "R14", "R15"};

static void printInfoReg() {
  int	   len = sizeof(_regNames) / sizeof(char *);
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

static int getPrefixForMemory(int size) {
  int prefix = 0;
  while ((size /= 1024) > 0)
    prefix++;
  return prefix;
}

static void printMemInfo() {
  memoryInfo *info = getMemoryInfo();

  printf("Total memory: %d B\n", info->totalMemory);
  printf("Used memory: %d B\n", info->usedMemory);
  printf("Free memory: %d B\n", info->freeMemory);

  /* int prefix = getPrefixForMemory(info->totalMemory);

  switch(prefix) {
    case 0: printf("Total memory: %d B\n", info->totalMemory); break;
    case 1: printf("Total memory: %d KB\n", info->totalMemory / 1024); break;
    case 2: printf("Total memory: %d MB\n", info->totalMemory / 1024 / 1024);
  break; default: printf("ERROR\n"); break;
  }

  prefix = getPrefixForMemory(info->usedMemory);

  switch(prefix) {
    case 0: printf("Used memory: %d B\n", info->usedMemory); break;
    case 1: printf("Used memory: %d KB\n", info->usedMemory / 1024); break;
    case 2: printf("Used memory: %d MB\n", info->usedMemory / 1024 / 1024);
  break; default: printf("ERROR\n"); break;
  }

  prefix = getPrefixForMemory(info->freeMemory);

  switch(prefix) {
    case 0: printf("Free memory: %d B\n", info->freeMemory); break;
    case 1: printf("Free memory: %d KB\n", info->freeMemory / 1024); break;
    case 2: printf("Free memory: %d MB\n", info->freeMemory / 1024 / 1024);
  break; default: printf("ERROR\n"); break;
  } */
}

static void ps() {
  processInfo **info = getProcessesInfo();
  int		i = 0;

  // Encabezados de la tabla
  printf(
      "PID\t\t\tPARENT PID\t\t\tPRIORITY\t\t\tSTATUS\t\t\tSTACK BASE\t\t\tSTACK POINTER\t\t\tNAME\n");

  while (info[i] != NULL) {
    printf("%d\t\t\t\t\t", info[i]->pid);
    printf("%d\t\t\t\t\t\t\t\t\t\t\t\t", info[i]->parentPid);
    printf("%d\t\t\t\t\t\t\t\t\t\t", info[i]->priority);
    switch (info[i]->status) {
      case 0: printf("RUNNING\t\t"); break;
      case 1: printf("BLOCKED\t\t"); break;
      case 3: printf("READY\t\t\t\t"); break;
      case 4: printf("ZOMBIE\t\t\t"); break;
      default: printf("UNKNOWN\t\t"); break;
    }
    printf("0x%x\t\t\t\t\t", info[i]->base);
    printf("0x%x\t\t\t\t\t\t\t\t", info[i]->current);
    printf("%s", info[i]->name);
    putchar('\n');
    i++;
  }
  freeProcessesInfo(info);
}

static void testProcesses() {
  char *helpArgs[] = {"help", NULL};
  int	pid = createProcess((uint16_t)1, (ProcessCode)&help, helpArgs, "test",
			    (uint8_t)6);
}

static void kill(char *pid) {
  if(killProcess((uint16_t)atoi(pid)) == -1)
    printErr("No se pudo matar el proceso\n");
}
