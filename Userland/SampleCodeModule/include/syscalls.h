#ifndef _SYSCALLS_H
#define _SYSCALLS_H

#include <color.h>
#include <stddef.h>
#include <stdint.h>
#include <types.h>

/**
 * @brief Escribe a partir del descriptor recibido un caracter
 * @param fd: FileDescriptor (STDOUT | STDERR)
 * @param c: Caracter a escribir
 */
void write(int fd, char c);

/**
 * @brief Lee un byte a partir del descriptor recibido
 * @param fd: FileDescriptor
 * @param buffer: Buffer donde se guarda el byte leido
 * @param count: Cantidad de bytes a leer
 * @return Byte leido
 */
int64_t read(int16_t fd, char *buffer, uint64_t count);

/**
 * @brief Devuelve la hora expresada en segundos
 * @return Hora expresada en segundos
 */
uint32_t getSeconds();

/**
 * @brief Pone todos los pixeles de la pantalla en negro y limpia el buffer de
 * video
 */
void clear(void);

/**
 * @brief
 * @param regarr: Vector donde se llena la informacion de los registros
 * @return Puntero a la informacion de los registros
 */
uint64_t *getInfoReg(uint64_t *regarr);

/**
 * @brief Cambia el tamaño de la fuente
 * @param size: (1|2|3)
 */
void setFontSize(uint8_t size);

/**
 * @brief Devuelve las dimensiones de la pantalla
 * @return 32 bits menos significativos el ancho, 32 el alto
 */
uint32_t getScreenResolution();

/**
 * @brief  Dibuja un rectangulo
 * @param  x: Origen en x
 * @param  y: Origen en y
 * @param  width: Ancho
 * @param  height: Alto
 * @param  color: Color de relleno
 */
void drawRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, Color color);

/**
 * @brief Devuelve la cantidad de ticks actuales
 * @return Cantidad de ticks
 */
uint64_t getTicks();

/**
 * @brief Llena un vector con 32 bytes de informacion a partir de una direccion
 * de memoria en hexa
 * @param pos: Direccion de memoria a partir de la cual se llena el vector
 * @param vec: Vector en el cual se llena la informacion
 */
void getMemory(uint64_t pos, uint8_t *vec);

/**
 * @brief Reproduce un sonido
 * @param frequency: Frecuencia del sonido
 * @param ticks: Cantidad de ticks que se desea que dure el sonido
 */
void playSound(uint64_t frequency, uint64_t ticks);

/**
 * @brief Ejecuta una excepcion de Invalid Opcode Exception
 */
void kaboom();

/**
 * @brief Establece un color de fuente
 * @param r: Color rojo
 * @param g: Color verde
 * @param b: Color azul
 */
void setFontColor(uint8_t r, uint8_t g, uint8_t b);

/**
 * @brief Retorna el color de fuente que se esta usando actualmente
 * @return Color
 */
Color getFontColor();

/**
 * @brief Reserva memoria dinamicamente
 * @param size: Cantidad de bytes a reservar
 * @return Puntero a la memoria reservada
 */
void *malloc(uint64_t size);

/**
 * @brief Libera memoria reservada dinamicamente
 * @param ptr: Puntero a la memoria a liberar
 */
void free(void *ptr);

/**
 * @brief Devuelve informacion sobre el estado de la memoria
 * @return Informacion sobre el estado de la memoria
 */
memoryInfo *getMemoryInfo();

/**
 * @brief Devuelve informacion sobre los procesos
 * @return Informacion sobre los procesos
 */
processInfo **getProcessesInfo();

/**
 * @brief Libera la memoria reservada para la informacion de los procesos
 * @param infoArray: Informacion de los procesos
 */
void freeProcessesInfo(processInfo **infoArray);

/**
 * @brief Crea un proceso
 * @param code: Codigo del proceso
 * @param args: Argumentos del proceso
 * @param name: Nombre del proceso
 * @param isForeground: Si es foreground o no
 * @param fds: File descriptors del proceso
 */
int createProcessFds(ProcessCode code, char **args, char *name, uint8_t isForeground, int fds[]);

/**
 * @brief Mata a un proceso
 *
 * @param pid: PID del proceso a matar
 * @return 0 si se pudo matar, -1 si no
 */
int killProcess(int16_t pid);

/**
 * @brief Cambia la prioridad de un proceso
 *
 * @param pid: PID del proceso a cambiar la prioridad
 * @param priority: Nueva prioridad
 */
int setPriority(int16_t pid, uint8_t priority);

/**
 * @brief Espera a que termine un proceso
 * @return PID del proceso actual
 */
int waitpid(int16_t pid);

/**
 * @brief Bloquea o desbloquea un proceso
 * @param pid: PID del proceso a bloquear o desbloquear
 */
int toggleBlockProcess(int16_t pid);

/**
 * @brief Devuelve el PID del proceso actual
 * @return PID del proceso actual
 */
int16_t getPid();

/**
 * @brief fuerza el cambio de contexto
 */
void yield();

/**
 * @brief Inicializa un semaforo
 * @param name: Nombre del semaforo
 * @param value: Valor inicial del semaforo
 * @return 0 si se pudo inicializar, -1 si no
 */
int semInit(char *name, uint32_t value);

/**
 * @brief Abre un semaforo
 * @param name: Nombre del semaforo
 * @return 0 si se pudo abrir, -1 si no
 */
int semOpen(const char *name);

/**
 * @brief Cierra un semaforo
 * @param name: Nombre del semaforo
 * @return 0 si se pudo cerrar, -1 si no
 */
int semClose(const char *name);

/**
 * @brief Destruye un semaforo
 * @param name: Nombre del semaforo
 * @return 0 si se pudo destruir, -1 si no
 */
int semWait(const char *name);

/**
 * @brief Incrementa el valor de un semaforo
 * @param name: Nombre del semaforo
 * @return 0 si se pudo incrementar, -1 si no
 */
int semPost(const char *name);

/**
 * @brief Abre un pipe
 * @param id: ID del pipe
 * @param mode: Modo de apertura
 * @param pid: PID del proceso que abre el pipe
 */
int openPipe(uint16_t id, uint8_t mode, uint16_t pid);

/**
 * @brief Cierra un pipe
 * @param id: ID del pipe
 * @param mode: Modo de cierre
 */
int closePipe(uint16_t id, uint16_t pid);

/**
 * @brief Desbloquea un proceso que esta bloqueado
 * @param pid: PID del proceso a desbloquear
 */
uint64_t unblockProcess(int16_t pid);

/**
 * @brief Hace un sleep de la cantidad de segundos recibidos
 * @param seconds: Cantidad de segundos a mimir
 */
void sleep(int seconds);

/**
 * @brief Devuelve el proximo ID de pipe disponible
 * @return Proximo ID de pipe disponible o NOT_FOUND si no hay
 */
int getNextPipeId();

/**
 * @brief Devuelve los fds del proceso actual. Siempre son 3.
 *
 * @return Arreglo con los FDS del proceso actual
 */
int *getFds();

void *openSharedMemory(int id, int size);

#endif