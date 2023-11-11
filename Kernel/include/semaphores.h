#ifndef _SEMAPHORES_H_
#define _SEMAPHORES_H_

#include <lib.h>
#include <queueADT.h>
#include <scheduler.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <types.h>

typedef int sem_t;

typedef struct Semaphore {
	char name[50];
	sem_t id;
	uint32_t value;
	int mutex;
	QueueADT blockedProcesses;
	QueueADT mutexProcesses;
} Semaphore;

void initializeSemaphores();

/**
 * @brief Crea un nuevo semaforo, indentificado por name y con valor inicial value
 *
 * @attention El nombre del semaforo debe ser unico
 *
 * @param name: Nombre con el que se identifica el semaforo
 * @param value: Valor inicial del semaforo
 * @return 0 si se creo correctamente, -1 si ya existe un semaforo con ese nombre
 */
int semInit(char * name, uint32_t value);

/**
 * @brief Abre un semaforo con el nombre name
 *
 * @param name: Nombre del semaforo
 * @return 0 si se abrio correctamente, -1 si no se pudo abrir
 */
int semOpen(const char * name);

/**
 * @brief Cierra el semaforo indicado
 *
 * @param name: nombre del semaforo a cerrar
 * @return 0 si se borro correctamente, -1 si no se pudo borrar
 */
int semClose(const char * name);

/**
 * @brief Incrementa el valor del semaforo indicado
 *
 * @param name: nombre del semaforo a incrementar
 * @return 0 si se incremento correctamente, -1 si no se pudo incrementar
 */
int semWait(const char * name);

/**
 * @brief Decrementa el valor del semaforo indicado
 *
 * @param name: nombre del semaforo a decrementar
 * @return 0 si se decremento correctamente, -1 si no se pudo decrementar
 */
int semPost(const char * name);

#endif