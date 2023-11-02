#include <linkedListADT.h>

typedef struct Node {
  void	      *data;
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

TNode addDataAtStart(LinkedListADT list, void *data) {
  if (list == NULL)
    return NULL;

  TNode newNode = (TNode)malloc(sizeof(Node));
  if (newNode == NULL)
    return NULL;

  newNode->data = data;
  return addNodeAtStart(list, newNode);
}

TNode addDataAtLast(LinkedListADT list, void *data) {
  if (list == NULL)
    return NULL;

  TNode newNode = (TNode)malloc(sizeof(Node));
  if (newNode == NULL)
    return NULL;

  newNode->data = data;
  return addNodeAtLast(list, newNode);
}

void *removeNode(LinkedListADT list, TNode node) {
  if (list == NULL || node == NULL)
    return NULL;

  if (list->first == node)
    list->first = node->next;
  else
    node->prev->next = node->next;

  list->len--;
  void *data = node->data;
  free(node);

  return data;
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

void *next(LinkedListADT list) {
  if (!hasNext(list))
    return NULL;

  void *data = list->current->data;
  list->current = list->current->next;
  return data;
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