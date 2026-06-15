
#include <getopt.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h> // To parse argument values


#include "consts.h"
#include "stb_include/stb_image.h"
#include "stb_include/stb_image_resize2.h"
#include "stb_include/stb_image_write.h"

#include "parse.h"
#include "threads/fit_thread.h"
#include "threads/prod_thread.h"
#include "threads/read_thread.h"
#include "tileset.h"
#include "commqueue.h"
#include "img_utils.h"



// ------- Utils functions ---------
static void push_images_to_queue(char *dir_path, comm_queue *cq) {
  char path[MAX_PATH_LEN];
  queue_elem to_push;

  DIR *input_dir = opendir(dir_path);
  if (input_dir == NULL) {
    printf("FATAL ERROR : Could not open the input directory");
    exit(EXIT_FAILURE);
  }

  struct dirent *dir_content;
  while ((dir_content = readdir(input_dir))) {
    if (dir_content->d_name[0] == '.') {
      continue;       // Ignore hidden + itself + parent
    }
    snprintf(path, sizeof(path), "%s/%s", dir_path, dir_content->d_name);    // Format current explore absolute path
    struct stat st;
    if (stat(path, &st)) {  // get current file path stats
      continue;         // if failed to get infos, continue
    }
    if (S_ISDIR(st.st_mode)) {
      push_images_to_queue(path, cq);    // recursively push content in sub-dirs
    }
    else if (S_ISREG(st.st_mode)) {
      to_push.filename = strdup(path);    // sets up queue argument
      cq_push(cq, to_push);              // push element to the queue
    }
  }
  closedir(input_dir);
}
// ---------------------------------



int main(int argc, char *argv[]) {
  // ---- Parameters parsing ----
  parameters p = parse_args(argc, argv);

  printf("Arguments parsing OK\nWill create mosaic with the following parameters\n");
  display_parameters(p);

  // ---- Source Image Load ----
  printf("\t-----\nLoading source image ...\n");
  int src_width;
  int src_height;
  int src_channels;       // number og 8 bit components in file (I force RGB : 3)
  uint8_t *src_img = stbi_load(p.input_image_path, &src_width, &src_height, &src_channels, 3);
  if (src_img == NULL) {
    printf("FATAL ERROR : Can't load source file\n");
    exit(EXIT_FAILURE);
  }

  // ---- Output image prepare ----
  unsigned int out_width = p.tileres * p.imres;          // set output width
  unsigned int out_height = (int) (((double)out_width * src_height / src_width) / p.tileres) * p.tileres;      // compute output height (as a multiple of tileres)
  printf("Output image will be %d x %d pixels (%d x %d tiles)\n\n", out_width, out_height, out_width/p.tileres, out_height/p.tileres);

  src_height = (int) ((double) src_width * out_height / out_width);       // crop source to maintain aspect ratio


  
  uint8_t *out_img = resize_img(src_img, src_width, src_height, out_width, out_height);
  if (out_img == NULL) {
    printf("FATAL ERROR : Could not resize image to output dimensions\nMaybe try to use less tiles (or less pixels in tiles) ?\n");
    exit(EXIT_FAILURE);
  }

  stbi_image_free(src_img);  // free source image

  unsigned int lookup_width = p.imres * p.matchres;
  unsigned int lookup_height = (int) (((double)lookup_width * out_height / out_width) / p.matchres) * p.matchres;    // compute as multiple of matchres
  uint8_t *lookup_img = resize_img(out_img, out_width, out_height, lookup_width, lookup_height);
  if (lookup_img == NULL) {
    printf("FATAL ERROR : Could not create small lookup image\n");
    exit(EXIT_FAILURE);
  }
  printf("Lookup dimensions = %d x %d\n", lookup_width, lookup_height);

  

  // ---- Parse tiles data ----
  printf("\t-----\nReading tiles in '%s'...\n", p.input_dir_path);
  tileset ts;
  ts_init(&ts);

    // Initialize the communication queue
  comm_queue parse_queue;
  cq_init(&parse_queue, QUEUES_CAPACITY);

    // Start the workers threads
  pthread_t *reader_threads = malloc((p.threads) * sizeof(pthread_t));
  read_thread_args *reader_args = malloc((p.threads) * sizeof(read_thread_args));
  if (reader_threads == NULL || reader_args == NULL) {
    printf("FATAL ERROR : Could not allocate pointers to threads and args\n");
    exit(EXIT_FAILURE);
  }
  for (unsigned int i = 0; i < p.threads; i+=1) {
    reader_args[i] = (read_thread_args) {&parse_queue, &ts, &p};
    pthread_create(&reader_threads[i], NULL, read_thread, &reader_args[i]);
  }

    // Populate the parse queue by walking the input dir
  push_images_to_queue(p.input_dir_path, &parse_queue);

    // Join the workers threads
  cq_close(&parse_queue);
  for (unsigned int i =0; i < p.threads; i +=1) {
    pthread_join(reader_threads[i], NULL);
  }
  free(reader_threads);
  free(reader_args);
  cq_destroy(&parse_queue);

  printf("Found %d tiles !\n", ts.size);

  if (ts.size <= p.variety*4) {
    printf("ERROR : Your variety parameter (-v %d) is too large for a tileset of %d pictures\n", p.variety, ts.size);
    p.variety = (ts.size / 8) + 1;
    printf("Your variety parameter has been forced to : %d\n", p.variety);
  }



  // ---- Build mosaic ----
  printf("\n\t-----\nBuilding mosaic ...\n");

    // Initialize the queues
  comm_queue request_queue;
  comm_queue answer_queue;
  cq_init(&request_queue, QUEUES_CAPACITY);
  cq_init(&answer_queue, QUEUES_CAPACITY);

    // Start the fitter threads
  pthread_t *fitter_threads = malloc((p.threads)*sizeof(pthread_t));
  fit_thread_args *fitter_args = malloc((p.threads)*sizeof(fit_thread_args));
  if (fitter_threads == NULL || fitter_args == NULL) {
    printf("FATAL ERROR : Could not allocate pointers to threads and args\n");
    exit(EXIT_FAILURE);
  }
  for (unsigned int i = 0; i < p.threads; i+=1) {
  // for (unsigned int i = 0; i < 1; i+=1) {
    fitter_args[i] = (fit_thread_args) {&request_queue, &answer_queue, &ts, &p};
    pthread_create(&fitter_threads[i], NULL, fit_thread, &fitter_args[i]);
  }

    // Start the producer thread
  pthread_t prod_t;
  prod_thread_args prod_args = (prod_thread_args) {&request_queue, &p, out_width/p.tileres, out_height/p.tileres, lookup_img};
  pthread_create(&prod_t, NULL, prod_thread, &prod_args);

    // Construct mosaic
  unsigned int *usage_tab = calloc(ts.size, sizeof(unsigned int));
  unsigned int tiles_left = (out_height/p.tileres) * (out_width/p.tileres);
  unsigned int last_checkpoint = 0;
  const unsigned int total_tiles = tiles_left;
  while (tiles_left) {
    queue_elem elem = cq_pop(&answer_queue);
    if (elem.out_best_fits == NULL) {
      printf("FATAL ERROR : Result answer is NULL when it should not\n");
    }

    // Find the best tile to use :
    // policy is : least used tile
    unsigned int best_index = elem.out_best_fits[0].tile_index;
    unsigned int best_usage = usage_tab[best_index];
    for (unsigned int i = 1; i < elem.out_best_fit_nb; i++) {
      unsigned int tmp_index = elem.out_best_fits[i].tile_index;
      if (usage_tab[tmp_index] < best_usage) {
        best_usage = usage_tab[tmp_index];
        best_index = tmp_index;
      }
    }

    // paste tile to output image
    paste_tile(out_img, out_width, ts.data[best_index].large_rgb, elem.x, elem.y, p);
    usage_tab[best_index] += 1;         // update usage
    free(elem.out_best_fits);      // free the heap res
    tiles_left--;                       // decrement remaining tiles

    // print a nice advancement indicator
    unsigned int checkpoint = tiles_left*100/total_tiles;
    if (checkpoint != last_checkpoint) {
      printf("Progress: %3d%%\r", 100 - checkpoint);
      fflush(stdout);
      last_checkpoint = checkpoint;
    }
  }


    // Join the producer thread
  pthread_join(prod_t, NULL);

    // Join the fitter threads
  for (unsigned int i = 0; i < p.threads; i += 1) {
    pthread_join(fitter_threads[i], NULL);
  }
  free(fitter_threads);
  free(fitter_args);

    // Destroy the working queues
  cq_destroy(&request_queue);
  cq_close(&answer_queue);
  cq_destroy(&answer_queue);



  // ---- Write output image ----
  printf("\nSaving '%s'...\n", p.output_image_path);
  if (!save_img(p.output_image_path, out_img, out_width, out_height)) {
  // if (!save_img(p.output_image_path, lookup_img, lookup_width, lookup_height)) {
    printf("FATAL ERROR : Could not write the final image to disk ...\n");
    exit(EXIT_FAILURE);
  }
  printf("Saved result to '%s'\n", p.output_image_path);

  // ---- Free all the resources
  ts_destroy(&ts);
  free(usage_tab);
  stbi_image_free(lookup_img);
  stbi_image_free(out_img);

return EXIT_SUCCESS;
}
