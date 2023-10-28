#include <memoryManager.h>

#define HEAP_SIZE    ((size_t)(2 * 1024))
#define HEAP_ADDRESS (0x600000)

typedef struct Node  // This is the structure of a free Node
{
  struct Node *next;  // The next free Node in the list.
  size_t       size;  // The size of the free Node.
} Node;

typedef struct Node *TNode;

Node first;

memoryInfo info;

void *malloc(size_t memAmount) {
  if (memAmount > info.freeMemory) {
    return NULL;
  }

  TNode currentNode = first.next;
  TNode previousNode = &first;

  while (currentNode != NULL && currentNode->size < memAmount) {
    previousNode = currentNode;
    currentNode = currentNode->next;
  }

  // Didnt find any Node big enough
  if (currentNode == NULL) {
    return NULL;
  }

  void *pointerToReturn;

  // Found a Node big enough
  if (currentNode->size - memAmount - sizeof(Node) > sizeof(Node)) {
    // Dividing the Node
    TNode newNode = (TNode)((uint64_t *)currentNode + sizeof(Node) + memAmount);
    newNode->size = currentNode->size - memAmount - sizeof(Node);
    newNode->next = currentNode->next;

    // Preparing the Node to be returned
    currentNode->size = memAmount;
    currentNode->next = NULL;

    info.freeMemory -= (memAmount + sizeof(Node));
    info.usedMemory += (memAmount + sizeof(Node));
    pointerToReturn = (uint64_t *)currentNode + sizeof(Node);
    previousNode->next = newNode;
  } else {
    pointerToReturn = (uint64_t *)currentNode + sizeof(Node);
    info.freeMemory -= currentNode->size;
    info.usedMemory += currentNode->size;
    previousNode->next = currentNode->next;
  }

  return pointerToReturn;
}

void free(void *ptr) {
  TNode nodeToFree = (TNode)((uint64_t *)ptr - sizeof(Node));
  int	nodeFreedSize = nodeToFree->size;

  TNode prev = &first;
  TNode curr = first.next;

  while (curr != NULL && (uint64_t *)curr < (uint64_t *)nodeToFree) {
    prev = curr;
    curr = curr->next;
  }

  // If the Node to be freed is the last one
  if (curr == NULL) {
    prev->next = nodeToFree;
    nodeToFree->next = NULL;
  } else {
    nodeToFree->next = curr;
    prev->next = nodeToFree;
  }

  // Merging to left
  if ((uint64_t *)prev + sizeof(Node) + prev->size == (uint64_t *)nodeToFree) {
    prev->size += nodeToFree->size + sizeof(Node);
    prev->next = nodeToFree->next;
    info.freeMemory += sizeof(Node);
    info.usedMemory -= sizeof(Node);
    nodeToFree = prev;
  }

  // Merging to right
  if (nodeToFree != NULL &&
      (uint64_t *)nodeToFree + sizeof(Node) + nodeToFree->size ==
	  (uint64_t *)nodeToFree->next) {
    nodeToFree->size += nodeToFree->next->size + sizeof(Node);
    nodeToFree->next = nodeToFree->next->next;
    info.freeMemory += sizeof(Node);
    info.usedMemory -= sizeof(Node);
  }

  info.freeMemory += nodeFreedSize;
  info.usedMemory -= nodeFreedSize;
}

void initializeMemoryManager() {
  first.next = (TNode)HEAP_ADDRESS;
  first.size = HEAP_SIZE;
  TNode firstNode = first.next;
  firstNode->size = HEAP_SIZE - sizeof(Node);
  info.freeMemory = firstNode->size;
  info.totalMemory = HEAP_SIZE;
  info.usedMemory = sizeof(Node);
  firstNode->next = NULL;
}

memoryInfo *getMemoryInfo() { return &info; }