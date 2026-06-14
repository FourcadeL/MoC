#ifndef FIT_THREAD_H
#define FIT_THREAD_H

#include "commqueue.h"
#include "tileset.h"
#include "parse.h"

typedef struct {
  comm_queue *inq;        // queue of input fit requests
  comm_queue *outq;       // queue of output fit answers
  tileset *ts;            // tileset to use
  parameters *p;          // application parameters
} fit_thread_args;

void *fit_thread(void *arg);

#endif // !FIT_THREAD_H
