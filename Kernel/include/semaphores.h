#ifndef _SEMAPHORES_H_
#define _SEMAPHORES_H_

#include <stdint.h>

typedef int sem_t;

typedef struct Semaphore {
    char name[50];
    sem_t id;
    uint32_t value;
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
int sem_init(const char *name, uint32_t value);

/**
 * @brief Abre un semaforo con el nombre name
 * 
 * @param name: Nombre del semaforo
 * @return 0 si se abrio correctamente, -1 si no se pudo abrir
 */
int sem_open(const char *name);

/**
 * @brief Cierra el semaforo indicado
 * 
 * @param name: nombre del semaforo a cerrar
 * @return 0 si se borro correctamente, -1 si no se pudo borrar
 */
int sem_close(const char *name);

#endif