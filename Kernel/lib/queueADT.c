#include <queueADT.h>

typedef struct Node {
  PCB	      *process;
  struct Node *next;
} Node;

typedef Node *TNode;

typedef struct QueueCDT {
  TNode first;
  TNode last;
  int	size;
} QueueCDT;

QueueADT createQueueADT() {
  QueueADT queue = (QueueADT)malloc(sizeof(QueueCDT));
  if (queue == NULL)
    return NULL;
  queue->size = 0;
  queue->first = NULL;
  queue->last = NULL;
  return queue;
}

int isEmpty(QueueADT queue) { return queue->size == 0; }

PCB *dequeue(QueueADT queue) {
  if (queue == NULL || queue->size == 0)
    return NULL;

  TNode node = queue->first;
  PCB  *process = node->process;
  queue->first = node->next;

  if (queue->first == NULL)
    queue->last = NULL;

  free(node);
  queue->size--;

  return process;
}

void enqueue(QueueADT queue, PCB *process) {
  if (queue == NULL)
    return;

  TNode node = (TNode)malloc(sizeof(Node));
  if (node == NULL)
    return;

  node->process = process;
  node->next = NULL;

  if (queue->last == NULL)
    queue->first = node;
  else
    queue->last->next = node;

  queue->last = node;

  queue->size++;
}

PCB *removeByPid(QueueADT queue, uint16_t pid) {
  if (queue == NULL || queue->size == 0)
    return NULL;

  TNode node = queue->first;
  TNode prev = NULL;
  PCB  *process = NULL;

  while (node != NULL && node->process->pid != pid) {
    prev = node;
    node = node->next;
  }

  if (node == NULL)
    return NULL;

  process = node->process;

  if (prev == NULL)
    queue->first = node->next;
  else
    prev->next = node->next;

  if (node == queue->last)
    queue->last = prev;

  free(node);
  queue->size--;

  return process;
}

static void freeRec(TNode node) {
  if (node == NULL)
    return;

  freeRec(node->next);
  free(node);
}

void freeQueue(QueueADT queue) {
  freeRec(queue->first);
  free(queue);
}