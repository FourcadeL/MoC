
#include "read_thread.h"
#include "commqueue.h"
#include "img_utils.h"
#include "stb_include/stb_image.h"
#include "stb_include/stb_image_resize2.h"
#include "stb_include/stb_image_write.h"
#include "tileset.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// The maximum image size we allow for parsing :
// 3000 x 3000 pixels (3bytes per pixel)
#define IMAGE_MAX_SIZE (3 * 3000 * 3000)

void *read_thread(void *a) {
  uint8_t *croped_image_buffer = malloc(IMAGE_MAX_SIZE * sizeof(uint8_t));

  // Get back arguments
  read_thread_args *args = a;
  comm_queue *cq = args->cq;
  tileset *ts = args->ts;
  parameters *p = args->p;

  queue_elem elem;

  // continue execution until the queue is closed
  while (1) {
    elem = cq_pop(cq);
    if (elem.filename == NULL) {
      break; // Exit the loop when last element of queue
    }

    int width;
    int height;
    int channels;
    ;
    uint8_t *img = stbi_load(elem.filename, &width, &height, &channels, 3);
    free(elem.filename);

    if (!img) { // If image is not opened -> don't bother
      continue;
    }

    // crop to square
    int m = width < height ? width : height;
    int x0 = (width - m) / 2;  // the x0 for square crop
    int y0 = (height - m) / 2; // the y0 for square crop

    if (m*m*3 > IMAGE_MAX_SIZE) {     // If croped image won't fit in the buffer (abort)
      stbi_image_free(img);
      continue;
    }

    // Do crop
    for (int row = 0; row < m; row += 1) {
      memcpy(croped_image_buffer + row * m * 3,
             img + ((row + y0) * width + x0) * 3, m * 3);
    }
    stbi_image_free(img);

    // create the two images (tile and lookup)
    uint8_t *lookup = resize_img(croped_image_buffer, m, m, p->matchres,
                                 p->matchres);
    uint8_t *tiledata = resize_img(croped_image_buffer, m, m, p->tileres, p->tileres);

    // it there was an error in tile create, just stop
    if (lookup == NULL || tiledata == NULL) {
      free(lookup);
      free(tiledata);
      continue;
    }
    // TODO: debug
    // stbi_write_jpg("atest.jpg", p->matchres, p->matchres, 3, lookup, 90);
    // stbi_write_jpg("btest.jpg", p->tileres, p->tileres, 3, tiledata, 90);

    // Ajoute la tile au tileset
    tile t = (tile) {lookup, tiledata};
    ts_add(ts, t);
  }

  free(croped_image_buffer);
  return NULL;
}
