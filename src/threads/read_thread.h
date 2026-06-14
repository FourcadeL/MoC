// Code of the reader threads

#ifndef READ_THREAD_H
#define READ_THREAD_H

#include "tileset.h"
#include "commqueue.h"
#include "parse.h"

typedef struct {
  comm_queue *cq;       // the communication queue providing the files
  tileset *ts;          // the current tileset
  parameters *p;  // The application parameters
} read_thread_args;

void *read_thread(void *arg);

#endif // !READ_THREAD_H
