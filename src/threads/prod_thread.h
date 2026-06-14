#ifndef PROD_THREAD_H
#define PROD_THREAD_H

#include "commqueue.h"
#include "parse.h"
#include <stdint.h>

typedef struct {
  comm_queue *req;          // fit request queue
  parameters *p;            // application parameters
  unsigned int x_nb;        // number of tiles on the x axis
  unsigned int y_nb;        // number of tiles on the y axis
  uint8_t *lookup_img;      // small resolution "lookup" image
} prod_thread_args;

void *prod_thread(void *arg);
#endif /* ifndef PROD_THREAD_H */
