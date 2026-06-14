// some utils routines to help image manipulation
//


#ifndef IMG_UTILS_H
#define IMG_UTILS_H

#include "parse.h"

#include <stdint.h>
// resize an image (auto malloc)
uint8_t *resize_img(const uint8_t *src_img, unsigned int src_w, unsigned int src_h, unsigned int out_w, unsigned int out_h);

// paste a tile on the output image
void paste_tile(uint8_t *out_canvas, unsigned int canvas_width, const uint8_t *tile, int x, int y, parameters p);

// save an image into the given path
int save_img(char *out_path, const uint8_t *image, unsigned int out_w, unsigned int out_h);
#endif // !IMG_UTILS_H
