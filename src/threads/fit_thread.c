// Code of the fitter thread

#include "fit_thread.h"
#include "tileheap.h"
#include "tileset.h"
#include "stb_include/stb_image.h"
#include "stb_include/stb_image_resize2.h"
#include "stb_include/stb_image_write.h"
#include <float.h>
#include <stdio.h>
#include <stdlib.h>

// Returns the difference with the current tile
// And bails out if the difference is too high
static inline double tile_diff(const uint8_t *t1, const uint8_t *t2,
                                int nb_pixels, double bail_out) {
    double diff = 0.0;
    for (int i = 0; i < nb_pixels; i++) {
        double r1=t1[i*3], g1=t1[i*3+1], b1=t1[i*3+2];
        double r2=t2[i*3], g2=t2[i*3+1], b2=t2[i*3+2];
        double ld = (0.2126*r1+0.7152*g1+0.0722*b1)-(0.2126*r2+0.7152*g2+0.0722*b2);
        diff += ld*ld*0.25;
        double dr=r1-r2, dg=g1-g2, db=b1-b2;
        diff += dr*dr + dg*dg + db*db;
        if (diff > bail_out) return DBL_MAX;
    }
    return diff;
}

void *fit_thread(void *a) {
  // Get arguments
  fit_thread_args *args = a;
  parameters *p = args->p;
  tileset *ts = args->ts;
  comm_queue *inq = args->inq;
  comm_queue *outq = args->outq;

  queue_elem elem;

  // continue execution until the queue is closed
  while(1) {
    elem = cq_pop(inq);
    if (elem.in_small_rgb_data == NULL) {
      break; // Exit the loop if last element of the queue
    }

    // Init best fit heap
    tile_heap best_heap;
    th_init(&best_heap, p->variety);

    double min = DBL_MAX;
    // Look for the best fit in the tileset
    for (unsigned int tile_index = 0; tile_index < ts->size; tile_index+=1) {
      double bail_out_value = th_get_max_score(&best_heap);
      double diff = tile_diff(elem.in_small_rgb_data, ts->data[tile_index].small_rgb, p->matchres*p->matchres, bail_out_value);

      // push to the heap (will automatically skip if larger then all the scores in the heap)
      th_push(&best_heap, (heap_elem) {diff, tile_index});
      if (diff < min) {
        min = diff;
      }
    }

    // Free the croped lookup
    free(elem.in_small_rgb_data);

    // get back the sorted heap elements
    elem.out_best_fits = th_get_sorted_elems(&best_heap);
    elem.out_best_fit_nb = best_heap.size;


    // puses the result to the queue
    cq_push(outq, elem);
  }

  return NULL;
}
