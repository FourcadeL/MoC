#include "consts.h"

#ifndef PARSE_H
#define PARSE_H

typedef struct {
  unsigned int threads;                   // the number of threads to use
  unsigned int tileres;                   // the resolution (in pixels) of each tiles
  unsigned int matchres;                  // the mathcing resolution for tile fitting
  unsigned int imres;                     // the width resolution (in number of tiles for the final image)
  unsigned int variety;                   // the variety parameter
  char input_image_path[MAX_PATH_LEN];    // the input image
  char output_image_path[MAX_PATH_LEN];   // the output impage
  char input_dir_path[MAX_PATH_LEN];      // the tiles input path
} parameters;


// ------------ Functions defs ------------
void display_usage();
void display_parameters(parameters param);
parameters init_parameters();
parameters parse_args(int argc, char *argv[]);

// ----------------------------------------

#endif // !PARSE_H

