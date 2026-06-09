// The tileset structure
//  contains all the tiles with small img data (for match)
//  and large img data (for creating the picture)

#ifndef TILESET_H
#define TILESET_H

#include <pthread.h>
#include <stdint.h>

#define TS_INIT_CAPACITY 1<<8

typedef struct {
  uint8_t *small_rgb;
  uint8_t *large_rgb;
} tile;

typedef struct {
  tile *data;
  unsigned int size;
  unsigned int capacity;
  pthread_mutex_t lock;
} tileset;

void ts_init(tileset *ts);
void ts_add(tileset *ts, tile t);
void ts_destroy(tileset *ts);

#endif // !TILESET_H
