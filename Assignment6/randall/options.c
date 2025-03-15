#include "options.h"

long long handle_nbytes(int argc, char **argv) {
  bool valid = false;
  long long nbytes;
  if (optind < argc) {
    char *endptr;
    errno = 0;
    nbytes = strtoll(argv[optind], &endptr, 10);
    if (errno)
      perror(argv[optind]);
    else
      valid = !*endptr && 0 <= nbytes;
  }
  if (!valid) {
    fprintf(stderr, "%s: usage: %s [-i input] [-o output] NBYTES\n", argv[0], argv[0]);
    return -1;
  }

  return nbytes;
}
