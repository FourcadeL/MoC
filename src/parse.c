// Arguments parser for the MoC utility

#include "parse.h"
#include "consts.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// ------------ Default Values ------------
#define DEFAULT_THREADS 4 // will be changed at execution
#define DEFAULT_TILERES 80
#define DEFAULT_MATCHRES 10
#define DEFAULT_IMRES 130
#define DEFAULT_VARIETY 30
#define DEFAULT_OUTPUT "mosaic.jpg"
// ----------------------------------------

void display_usage() {
  printf("MoC : a simple mosaic creator in C\n");
  printf("usage : moc [-h] [-o OUTPUT] [-t THREADS] [-l TILERES] [-m "
         "MATCHRES] [-r IMRES] [-v VARIETY] image tiles_directory\n");
  printf("positional arguments:\n");
  printf("\timage                 Input image to transform\n");
  printf("\ttiles_directory       Directory for the tiles data\n");
  printf("\noptions:\n");
  printf("\t-h, --help            show this help message and exit\n");
  printf("\t--output, -o OUTPUT   The output image\n");
  printf("\t--threads, -t THREADS\n");
  printf("\t                      The number of threads to use (computed "
         "default is 8)\n");
  printf("\t--tileres, -l TILERES\n");
  printf("\t                      The resolution (in pixels) of the tiles "
         "(default is %d)\n", DEFAULT_TILERES);
  printf("\t--matchres, -m MATCHRES\n");
  printf("\t                      Tile matching resolution (default is %d)\n", DEFAULT_MATCHRES);
  printf("\t--imres, -r IMRES     The horizontal resolution of resulting image "
         "in number of tiles (default is %d)\n", DEFAULT_IMRES);
  printf("\t--variety, -v VARIETY\n");
  printf("\t                      The variety parameter to avoid reusing the "
         "same tile (default is %d)\n", DEFAULT_VARIETY);
}

void display_parameters(parameters param) {
  printf("Requested parameters :\n");
  printf("\tinput path : %s\n", param.input_image_path);
  printf("\tinput dir : %s\n", param.input_dir_path);
  printf("\toutput path : %s\n", param.output_image_path);
  printf("\tnb threads : %u\n", param.threads);
  printf("\ttileres : %u\n", param.tileres);
  printf("\tmatchres : %u\n", param.matchres);
  printf("\timres : %u\n", param.imres);
  printf("\tvariety : %u\n", param.variety);
}

// Initialize a parameters struct with default parameters values
parameters init_parameters() {
  parameters ret;
  ret.threads =
      (int)sysconf(_SC_NPROCESSORS_ONLN); // get system number of cores
  ret.tileres = DEFAULT_TILERES;          // default tile size in pixels
  ret.matchres = DEFAULT_MATCHRES;        // default tile matching resolution
  ret.imres = DEFAULT_IMRES; // default horizontal resolution (in tiles) of the
                             // final image
  ret.variety = DEFAULT_VARIETY; // default variety parameter
  ret.input_image_path[0] = '\0';
  ret.input_dir_path[0] = '\0';
  strncpy(ret.output_image_path, DEFAULT_OUTPUT, MAX_PATH_LEN - 1);
  return ret;
}

parameters parse_args(int argc, char *argv[]) {
  parameters ret = init_parameters();
  int opt;
  while ((opt = getopt(argc, argv, "ho:t:l:m:r:v:")) != -1) {
    switch (opt) {
    case 'h':
      display_usage();
      exit(EXIT_FAILURE);
    case 'o':
      strncpy(ret.output_image_path, optarg, MAX_PATH_LEN-1);
      break;
    case 't':
      ret.threads = atoi(optarg);
      break;
    case 'l':
      ret.tileres = atoi(optarg);
      break;
    case 'm':
      ret.matchres = atoi(optarg);
      break;
    case 'r':
      ret.imres = atoi(optarg);
      break;
    case 'v':
      ret.variety = atoi(optarg);
      break;
    default:
      printf("Unknown argument %c !\n", opt);
      display_usage();
      exit(EXIT_FAILURE);
    }
  }

  // now parse the remaining input arguments
  if (optind + 1 >= argc) {                 // test that we still have the two mandatory arguments left to parse
    printf("Missing arguments\n");
    display_usage();
    exit(EXIT_FAILURE);
  }


  // correct what could be wrong arguments
  if (ret.variety < 1) {
    ret.variety = 1;
  }
  if (ret.matchres < 1 || ret.matchres > ret.tileres) {
    ret.matchres = DEFAULT_MATCHRES;
  }


  strncpy(ret.input_image_path, argv[optind++], MAX_PATH_LEN-1);  // get input image
  strncpy(ret.input_dir_path, argv[optind], MAX_PATH_LEN-1);

  return ret;
}
