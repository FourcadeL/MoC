
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // To parse argument values

typedef struct {
  unsigned int threads;
  unsigned int tileres;
  unsigned int matchres;
  unsigned int imres;
  unsigned int variety;
} parameters;

// ------- Utils functions ---------
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
         "(default is 50)\n");
  printf("\t--matchres, -m MATCHRES\n");
  printf("\t                      Tile matching resolution (default is 5)\n");
  printf("\t--imres, -r IMRES     The horizontal resolution of resulting image "
         "in number of tiles (default is 50)\n");
  printf("\t--variety, -v VARIETY\n");
  printf("\t                      The variety parameter to avoid reusing the "
         "same tile (default is 40)\n");
}

void display_parameters(parameters param) {
  printf("Requested parameters :\n");
  printf("\tinput path : (TODO)\n");
  printf("\tinput dir : (TODO)\n");
  printf("\toutput path : (TODO)\n");
  printf("\tnb threads : %u\n", param.threads);
  printf("\ttileres : %u\n", param.tileres);
  printf("\tmatchres : %u\n", param.matchres);
  printf("\timres : %u\n", param.imres);
  printf("\tvariety : %u\n", param.variety);
}

// Initialize a parameters struct with default parameters values
parameters init_parameters() {
  parameters ret;
  ret.threads = 4; // TODO: get from sys
  ret.tileres = 50; // default tile size in pixels
  ret.matchres = 5; // default tile matching resolution
  ret.imres = 50; // default horizontal resolution (in tiles) of the final image
  ret.variety = 40; // default variety parameter
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
        // TODO: handle output string
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
  return ret;
}

// ---------------------------------



int main(int argc, char *argv[]) {
  display_parameters(parse_args(argc, argv));
  // display_usage();

  return EXIT_SUCCESS;
}
