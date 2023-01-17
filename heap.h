#ifndef HEAP_H
#define HEAP_H

typedef struct heap_node heap_node_t;

struct heap_node{
  int weight;
  int index;
  int x;
  int y;
};

typedef struct heap{
  heap_node_t *top;
  heap_node_t *q; //*q because it will be given a dynamic size upon init
  int size;
}heap_t;

int heap_init(heap_t *h, int n);
int heap_deinit(heap_t *h);
int heap_add(heap_t *h, int weight, int x, int y);
heap_node_t heap_pop(heap_t *h);
void heapify_up(heap_t *h, heap_node_t *n);
void heapify_down(heap_t *h, heap_node_t *n);
void swap(heap_node_t *a, heap_node_t *b);
int heap_sortify(heap_t *h);
int is_empty(heap_t *h);


#endif
