#include "img_utils.h"
#include "parse.h"

#include "stb_include/stb_image.h"
#include "stb_include/stb_image_resize2.h"
#include "stb_include/stb_image_write.h"
#include <string.h>

uint8_t *resize_img(const uint8_t *src_img, unsigned int src_w,
                    unsigned int src_h, unsigned int out_w,
                    unsigned int out_h) {
  uint8_t *out = malloc(out_w * out_h * 3 * sizeof(uint8_t));
  // uint8_t *out = NULL;
  stbir_resize_uint8_srgb(src_img, src_w, src_h, src_w * 3, out, out_w, out_h,
                          out_w * 3, STBIR_RGB);
  return out;
}

int save_img(char *out_path, const uint8_t *image, unsigned int out_w,
             unsigned int out_h) {
  char *extension = strrchr(out_path, '.');
  if (extension && !strcasecmp(extension, ".png")) {
    return stbi_write_png(out_path, out_w, out_h, 3, image, out_w * 3);
  } else if (extension && !strcasecmp(extension, ".bmp")) {
    return stbi_write_bmp(out_path, out_w, out_h, 3, image);
  } else {
    // sets up correct extension
    strcpy(extension, ".jpg");
    // write as jpg
    return stbi_write_jpg(out_path, out_w, out_h, 3, image, 90);
  }
}

void paste_tile(uint8_t *out_canvas, unsigned int canvas_width,
                const uint8_t *tile, int x, int y, parameters p) {
  for (unsigned int row = 0; row < p.tileres; row++) {
    memcpy(out_canvas + ((y + row) * canvas_width + x) * 3, tile + row * p.tileres * 3,
           p.tileres * 3);
  }
}
