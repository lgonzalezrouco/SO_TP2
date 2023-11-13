// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <memoryManager.h>
#ifndef BUDDY

typedef struct Node {
	void *address;
	uint64_t size;
	bool isFree;
	struct Node *prev;
	struct Node *next;
} Node;

typedef Node *NodePtr;

Node memoryList;

memoryInfo memoryData;

void *listAddress;

void initializeMemoryManager() {
	listAddress = (void *) HEAP_ADDRESS_LIST;
	memoryList.address = (void *) HEAP_ADDRESS;
	memoryList.size = HEAP_SIZE;
	memoryList.isFree = true;
	memoryList.prev = NULL;
	memoryList.next = NULL;

	memoryData.totalMemory = HEAP_SIZE;
	memoryData.freeMemory = HEAP_SIZE;
	memoryData.usedMemory = 0;
}

void *malloc(uint64_t size) {
	if (size <= 0)
		return NULL;

	NodePtr current = &memoryList;

	while (current != NULL && !(current->isFree && current->size >= size))
		current = current->next;

	if (current == NULL)
		return NULL;

	NodePtr new_node = (NodePtr) (listAddress + sizeof(struct Node));

	listAddress += sizeof(struct Node);
	new_node->address = current->address + size;
	new_node->size = current->size - size;
	new_node->isFree = true;
	new_node->prev = current;
	new_node->next = current->next;

	current->size = size;
	current->isFree = false;
	current->next = new_node;

	memoryData.freeMemory -= size;
	memoryData.usedMemory += size;

	return current->address;
}

void free(void *address) {
	NodePtr current = &memoryList;

	while (current != NULL && current->address != address)
		current = current->next;

	if (current == NULL || current->isFree)
		return;

	current->isFree = true;

	memoryData.freeMemory += current->size;
	memoryData.usedMemory -= current->size;

	if (current->prev != NULL && current->prev->isFree) {
		current->prev->size += current->size;
		current->prev->next = current->next;
		current->next->prev = current->prev;
		current = current->prev;
	}

	if (current->next != NULL && current->next->isFree) {
		current->size += current->next->size;
		current->next = current->next->next;
		current->next->prev = current;
	}
}

memoryInfo *getMemoryInfo() {
	return &memoryData;
}
#endif