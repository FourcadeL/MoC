
#include <stdlib.h>
#include <unistd.h>       // To parse argument values


int main(int argc, char *argv[])
{
  int opt;
  while ((opt = getopt(argc, argv, "ld")) != 1) {

  }
  return EXIT_SUCCESS;
}
