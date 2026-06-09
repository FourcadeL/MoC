
#include "tileset.h"
#include <asm-generic/errno-base.h>
#include <pthread.h>
#include <stdlib.h>

// ------------ Utils functions ----------
static void tileset_expand_size(tileset *ts) {
  ts->capacity = ts->capacity*2;
  ts->data = reallocarray(ts->data, ts->capacity, sizeof(tile));
}
// ---------------------------------------


void ts_init(tileset *ts) {
  ts->data = calloc(TS_INIT_CAPACITY, sizeof(tile));
  ts->size = 0;
  ts->capacity = TS_INIT_CAPACITY;
  pthread_mutex_init(&ts->lock, NULL);
}

void ts_add(tileset *ts, tile t) {
  pthread_mutex_lock(&ts->lock);
  if (ts->size >= ts->capacity) {             // if we are at the limit of the tileset size doubles it
    tileset_expand_size(ts);
  }
  ts->data[ts->size++] = t;                   // set tile and size
  pthread_mutex_unlock(&ts->lock);
}

void ts_destroy(tileset *ts) {
  free(ts->data);
  while (pthread_mutex_destroy(&ts->lock) == EBUSY) {
    pthread_mutex_lock(&ts->lock);
    pthread_mutex_unlock(&ts->lock);
  }
}
