// Code for producer thread
// This thread will just produce the fit requests for all tiles in the main image

#include "prod_thread.h"
#include "stb_include/stb_image.h"
#include "stb_include/stb_image_resize2.h"
#include "stb_include/stb_image_write.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void *prod_thread(void *a) {
  // Get arguments
  prod_thread_args *args = a;
  parameters *p = args->p;
  unsigned int lookup_width = p->imres * p->matchres;
  unsigned int match_width = p->matchres;
  // unsigned int match_height = p->matchres;

  for (unsigned int i = 0; i < args->x_nb; i +=1) {
    for (unsigned int j = 0; j < args->y_nb; j +=1) {
      unsigned int x_tile = i*p->matchres;
      unsigned y_tile = j*p->matchres;

      // crop the part of the lookup image into a buffer
      uint8_t *crop = malloc(match_width*match_width*3*sizeof(uint8_t));
      if (crop == NULL) {
        printf("FATAL in producer thread : Could not allocate lookup bufer\n");
        exit(EXIT_FAILURE);
      }
      for (unsigned int row = 0; row < match_width; row+=1) {
        memcpy(crop+row*match_width*3,
               args->lookup_img+((y_tile+row)*lookup_width+x_tile)*3,
               match_width*3);
      }

      
      // push element to queue
      queue_elem push_elem = (queue_elem) {i*p->tileres, j*p->tileres, crop, NULL, 0, NULL};
      cq_push(args->req, push_elem);
    }
  }

  cq_close(args->req);       // close the queue
  return NULL;
}
