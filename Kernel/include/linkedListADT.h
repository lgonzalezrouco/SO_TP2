#ifndef _LINKEDLISTADT_H_
#define _LINKEDLISTADT_H_

#include <memoryManager.h>
#include <types.h>

typedef struct LinkedListCDT *LinkedListADT;
typedef struct Node	     *TNode;

/**
 * @brief Crea una LinkedListADT
 *
 * @return LinkedListADT
 */
LinkedListADT createLinkedListADT();

/**
 * @brief Crea un nodo y lo agrega al principio de la lista
 *
 * @param list
 * @param data
 * @return El nodo agregado
 */
TNode addDataAtStart(LinkedListADT list, void *data);

/**
 * @brief Crea un nodo y lo agrega al final de la lista
 *
 * @param list
 * @param data
 * @return El nodo agregado
 */
TNode addDataAtLast(LinkedListADT list, void *data);

/**
 * @brief Agrega un nodo al principio de la lista
 *
 * @param list
 * @param node
 * @return El nodo agregado
 */
TNode addNodeAtStart(LinkedListADT list, TNode node);

/**
 * @brief Agrega un nodo al final de la lista
 *
 * @param list
 * @param node
 * @return El nodo agregado
 */
TNode addNodeAtLast(LinkedListADT list, TNode node);

/**
 * @brief Elimina un nodo de la lista
 *
 * @param list
 * @param node
 * @return La data del nodo eliminado
 */
void *removeNode(LinkedListADT list, PCB * process);

/**
 * @param list
 * @return El primer nodo de la lista
 */
TNode getFirst(LinkedListADT list);

/**
 * @param list
 * @return Si la lista esta vacia
 */
int isEmpty(LinkedListADT list);

/**
 * @param list
 * @return El tamaño de la lista
 */
int getSize(LinkedListADT list);

/**
 * @brief Inicializa el iterador de la lista
 *
 * @param list
 */
void begin(LinkedListADT list);

/**
 * @brief Verifica si hay un siguiente elemento en la lista
 *
 * @param list
 * @return Si hay un siguiente elemento
 */
int hasNext(LinkedListADT list);

/**
 * @brief Devuelve el siguiente elemento de la lista
 *
 * @param list
 * @return El siguiente elemento
 */
void *next(LinkedListADT list);

/**
 * @brief Libera la memoria de la lista
 *
 * @param list
 */
void freeList(LinkedListADT list);

#endif