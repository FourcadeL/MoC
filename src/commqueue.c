#include "commqueue.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

static comm_queue *cq_init(unsigned int capacity, unsigned int nb_workers) {
  comm_queue *res = malloc(sizeof(comm_queue));
  res->content = calloc(capacity, sizeof(queue_elem));
  res->capacity = capacity;
  res->head = 0;
  res->tail = 0;
  res->size = 0;
  res->workers_nb = nb_workers;
  res->active_workers = nb_workers;
  pthread_mutex_init(&res->lock, NULL);
  pthread_cond_init(&res->not_empty, NULL);
  pthread_cond_init(&res->not_full, NULL);

  return res;
}

static void cq_reset(comm_queue *q) {
  pthread_mutex_lock(&q->lock);
  if (q->active_workers != 0) {
    fprintf(stderr,"FATAL ERROR: cannot reset comm queue, some workers did not finished\n");
    exit(EXIT_FAILURE);
  }
  q->head = 0;
  q->tail = 0;
  q->size = 0;
  q->active_workers = q->workers_nb;
  pthread_cond_init(&q->not_empty, NULL);
  pthread_cond_init(&q->not_full, NULL);
  pthread_mutex_unlock(&q->lock);
}

static void cq_destroy(comm_queue *q) {
  pthread_mutex_lock(&q->lock);
  if (q->active_workers != 0) {
    fprintf(stderr,"FATAL ERROR: cannot destroy comm queue, some workers did not finished\n");
    exit(EXIT_FAILURE);
  }
  pthread_mutex_unlock(&q->lock);
  free(q->content);
  free(q);
}

static void cq_push(comm_queue *q, queue_elem elem) {
  pthread_mutex_lock(&q->lock);
  while (q->capacity >= q->size) {          // If the queue is full, wait on the not_full cond
    pthread_cond_wait(&q->not_full, &q->lock);   // Automatically unlocks lock and relocks it on reentering
  }
  q->content[q->tail] = elem;               // Put content in
  q->tail = (q->tail+1) % q->capacity;      // Increment circlyng buffer pointer
  q->size++;                                // Increment size
  pthread_cond_signal(&q->not_empty);
  pthread_mutex_unlock(&q->lock);
}

static queue_elem cq_pop(comm_queue *q) {
  pthread_mutex_lock(&q->lock);
  while (q->size <= 0) {                    // If the queue is empty, wait on the not_empty cond
    pthread_cond_wait(&q->not_empty, &q->lock);
  }
  queue_elem out = q->content[q->head];    // Remember output pointer
  q->head = (q->head+1) % q->capacity;      // Increment circlyng buffer pointer
  q->size--;
  pthread_cond_signal(&q->not_full);
  pthread_mutex_unlock(&q->lock);
  return out;
}


