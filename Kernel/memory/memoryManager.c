// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <memoryManager.h>
#ifndef BUDDY

#define HEAP_SIZE    ((size_t)(2 * 1024))
#define HEAP_ADDRESS (0x600000)

typedef struct Node {
	void *address;
	uint64_t size;
	bool is_free;
	struct Node *prev;
	struct Node *next;
} Node;

typedef Node *NodePtr;

Node memory_list;

memoryInfo memory_data;

void *list_address;

void initializeMemoryManager() {
	list_address = (void *) 0x600000;
	memory_list.address = (void *) (0x600000 + 0x100000);
	memory_list.size = 0x1000000;
	memory_list.is_free = true;
	memory_list.prev = NULL;
	memory_list.next = NULL;

	memory_data.totalMemory = 0x1000000;
	memory_data.freeMemory = 0x1000000;
	memory_data.usedMemory = 0;
}

void *malloc(uint64_t size) {
	if (size <= 0) {
		return NULL;
	}
	NodePtr current = &memory_list;
	while (current != NULL && !(current->is_free && current->size >= size)) {
		current = current->next;
	}

	if (current == NULL) {
		return NULL;  // There is no more empty space
	}

	NodePtr new_node = (NodePtr) (list_address + sizeof(struct Node));
	list_address += sizeof(struct Node);
	new_node->address = current->address + size;
	new_node->size = current->size - size;
	new_node->is_free = true;
	new_node->prev = current;
	new_node->next = current->next;

	current->size = size;
	current->is_free = false;
	current->next = new_node;

	memory_data.freeMemory -= size;
	memory_data.usedMemory += size;

	return current->address;
}

void free(void *address) {
	NodePtr current = &memory_list;
	while (current != NULL && current->address != address) {
		current = current->next;
	}

	if (current == NULL)
		return;  // Address not found

	if (current->is_free)
		return;  // Address already free

	current->is_free = true;

	memory_data.freeMemory += current->size;
	memory_data.usedMemory -= current->size;

	if (current->prev != NULL && current->prev->is_free) {
		current->prev->size += current->size;
		current->prev->next = current->next;
		current->next->prev = current->prev;
		current = current->prev;
	}

	if (current->next != NULL && current->next->is_free) {
		current->size += current->next->size;
		current->next = current->next->next;
		current->next->prev = current;
	}
	
}

memoryInfo *getMemoryInfo() {
	return &memory_data;
}
#endif