#ifndef _QUEUE_ADT_H_
#define _QUEUE_ADT_H_

#include "memoryManager.h"
#include "types.h"

typedef struct QueueCDT *QueueADT;

/**
 * @brief Crea una QueueADT
 *
 * @return QueueADT
 */
QueueADT createQueueADT();

/**
 * @param queue
 * @return Si la cola esta vacia
 */
int isEmpty(QueueADT queue);

/**
 * @brief Devuelve el primer elemento de la cola
 *
 * @param queue
 * @return PCB*
 */
PCB *dequeue(QueueADT queue);

/**
 * @brief Agrega un elemento al final de la cola
 *
 * @param queue
 * @param process
 */
void enqueue(QueueADT queue, PCB *process);

/**
 * @brief Remueve un elemento especifico de la cola por su pid
 *
 * @param queue
 * @param pid
 * @return PCB*
 */
PCB *removeByPid(QueueADT queue, uint16_t pid);

/**
 * @brief Libera la memoria de la cola
 *
 * @param queue
 */
void freeQueue(QueueADT queue);

#endif