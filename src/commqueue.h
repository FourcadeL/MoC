// The commqueue serves to communicate information both ways :
//  - from the producer thread to the workers threads (tile position and small rgb correspondance info)
//  - from the worker threads to the collector threads (tile position , best fit indices and best fit number)
// also, elements are reused to contain filename to use when parsing


#include "tileheap.h"
#include <pthread.h>
#include <stdint.h>

#ifndef COMM_QUEUE_H
#define COMM_QUEUE_H

typedef struct {
  int x, y;     // the x y position of the tile
  uint8_t *in_small_rgb_data;
  heap_elem *out_best_fits;
  unsigned int out_best_fit_nb;
  char *filename;
} queue_elem;

typedef struct {
  queue_elem *content;             // Table of pointers to queue_elem
  unsigned int capacity, head, tail, size;
  unsigned int closed;            // 0 -> queue is active | 1 -> queue is closed
  pthread_mutex_t lock;
  pthread_cond_t not_empty, not_full;
} comm_queue;

// ------------ Functions defs ------------
// comm_queue *cq_init(unsigned int capacity);
void cq_init(comm_queue *q, unsigned int capacity);
void cq_reset(comm_queue *q);
void cq_close(comm_queue *q);
void cq_destroy(comm_queue *q);

void cq_push(comm_queue *q, queue_elem elem);
queue_elem cq_pop(comm_queue *q);
// ----------------------------------------

#endif // !COMM_QUEUE_H
