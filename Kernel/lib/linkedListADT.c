#include <linkedListADT.h>

typedef struct Node {
  PCB	      *process;
  struct Node *prev;
  struct Node *next;
} Node;

typedef struct LinkedListCDT {
  Node *first;
  Node *last;
  Node *current;
  int	len;
} LinkedListCDT;

static void freeRec(TNode node);

LinkedListADT createLinkedListADT() {
  LinkedListADT list = (LinkedListADT)malloc(sizeof(LinkedListCDT));
  if (list == NULL)
    return NULL;

  list->len = 0;
  list->first = NULL;
  list->last = NULL;
  list->current = NULL;
  return list;
}

TNode addNodeAtStart(LinkedListADT list, TNode node) {
  if (list == NULL)
    return NULL;

  node->prev = NULL;

  if (list->len > 0)
    list->first->prev = node;
  else
    list->last = node;

  node->next = list->first;
  list->first = node;
  list->len++;

  return node;
}

TNode addNodeAtLast(LinkedListADT list, TNode node) {
  if (list == NULL)
    return NULL;

  node->next = NULL;

  if (list->len > 0)
    list->last->next = node;
  else
    list->first = node;

  node->prev = list->last;
  list->last = node;
  list->len++;

  return node;
}

TNode addProcessAtStart(LinkedListADT list, PCB *process) {
  if (list == NULL)
    return NULL;

  TNode newNode = (TNode)malloc(sizeof(Node));
  if (newNode == NULL)
    return NULL;

  newNode->process = process;
  return addNodeAtStart(list, newNode);
}

TNode addProcessAtLast(LinkedListADT list, PCB *process) {
  if (list == NULL)
    return NULL;

  TNode newNode = (TNode)malloc(sizeof(Node));
  if (newNode == NULL)
    return NULL;

  newNode->process = process;
  return addNodeAtLast(list, newNode);
}

PCB *removeNode(LinkedListADT list, PCB *process) {
  if (list == NULL || process == NULL)
    return NULL;

  TNode nodeToRemove = list->first;
  while (nodeToRemove != NULL && nodeToRemove->process != process)
    nodeToRemove = nodeToRemove->next;

  if (list->first == nodeToRemove)
    list->first = nodeToRemove->next;
  else
    nodeToRemove->prev->next = nodeToRemove->next;

  list->len--;
  PCB *process = nodeToRemove->process;
  free(nodeToRemove);

  return process;
}

TNode getFirst(LinkedListADT list) {
  if (list == NULL)
    return NULL;

  return list->first;
}

int isEmpty(LinkedListADT list) {
  if (list == NULL)
    return -1;

  return list->len == 0;
}

int getSize(LinkedListADT list) {
  if (list == NULL)
    return -1;

  return list->len;
}

// Para usar de forma iterativa
void begin(LinkedListADT list) {
  if (list == NULL)
    return;

  list->current = list->first;
}

int hasNext(LinkedListADT list) {
  if (list == NULL)
    return -1;

  return list->current != NULL;
}

PCB *next(LinkedListADT list) {
  if (!hasNext(list))
    return NULL;

  PCB *process = list->current->process;
  list->current = list->current->next;
  return process;
}

// Para liberar
static void freeRec(TNode node) {
  if (node == NULL)
    return;

  freeRec(node->next);
  free(node);
}

void freeList(LinkedListADT list) {
  freeRec(list->first);
  free(list);
}