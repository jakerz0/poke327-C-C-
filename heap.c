#include <stdlib.h>
#include <stdio.h>
#include "heap.h"

int heap_init(heap_t *h, int n){
  h -> top = NULL;
  h -> q = (heap_node_t *)malloc( sizeof(*h->q) * (n + 1)); //make an array the size of a node * n of elements
  h -> size = 0;
  return 0;
}

int heap_deinit(heap_t *h){
  free(h->q); //free the original reference is all to do
  return 0;
}

int heap_add(heap_t *h, int weight, int x, int y){

  int size;
  //if(!(h->q[size] = malloc( sizeof(h->q[size])))) return 1
  h->size++;
  size = h->size;
  
  //printf("w%d x%d y%d\n",weight,x,y);
  
  h->q[size].weight = weight;
  //printf("%d\n", h->q[size].weight);
  h->q[size].index = size;
  h->q[size].x = x;
  h->q[size].y = y;

  if(!h->top){ //if there is no top in the heap, set the newly added node, the first, to the top
    h->top = &h->q[size];
  }
  
  //printf("size = %d\n", h->size);

  return 0;
}

/*
  swaps the values of the nodes, not the nodes themselves
 */
void swap(heap_node_t *a, heap_node_t *b){
  heap_node_t tmp;

  tmp = *a;
  
  *a = *b;
  *b = tmp;

  b->index = a->index;
  a->index = tmp.index;
}

heap_node_t heap_pop(heap_t *h){
  if(h->size == 1){
    h->size--;
    //printf("size: %d\n",h->size);
    return h->q[1];
  }
  heap_node_t tmp;

  heap_node_t last = h->q[h->size];

  tmp = h->q[1];

  //printf("tmp ind-%d x-%d y-%d weight-%d\n",tmp.index,tmp.x,tmp.y,tmp.weight);

  h->q[1] = last;

  h->q[1].index = 1;

  //last = NULL;

  h->size--;

  return tmp;
}

void heapify_up(heap_t *h, heap_node_t *n){
  int parent;
  if(h->size <= 1) return; //if size is 1 or 0 do nothing

  if(n->index < 1) return; //if the node is the top
  parent = n->index / 2;

  if(h->q[parent].weight > n->weight){
    swap(n, &h->q[parent]);
    heapify_up(h, &h->q[parent]);
  }

}

void heapify_down(heap_t *h, heap_node_t *n){
  int j; //index of who we want to swap with
  int size = h->size;
  
  if(n->index * 2 > size) return; //if no left child, return
  
  //if there is no right child, plan swap with left
  if(n->index * 2 + 1 > size) j = n->index * 2; 
  //if right exists and left smaller than right
  else if(h->q[n->index * 2].weight < h->q[n->index * 2 + 1].weight) j = n->index * 2;
  //if right is smaller than left
  else j = n->index * 2 + 1;

  if(n->weight < h->q[j].weight) return; //if n is less than both kids, return

  //printf("j is %d\n", j);
  swap(n, &h->q[j]); //swapping the nodes
  
  heapify_down(h, &h->q[j]);//heapify down with new location of n (n's information)
}

int heap_sortify(heap_t *h){
  int cur = h->size / 2;

  //printf("AAAAAAAAAAAA %d\n",cur);

  while(cur > 0){
    heapify_down(h, &h->q[cur]);
    cur--;
  }

  return 0;
}

int is_empty(heap_t *h){
  return !(h->size); //if it is 0, we return true 
}

