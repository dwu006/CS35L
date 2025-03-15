#include "rand64-sw.h"
/* Software implementation.  */

/* Input stream containing random bytes.  */
FILE *urandstream = NULL; // pointer to the file stream used to read random data
static const char *file_path = "/dev/random"; // default file path
struct drand48_data state;

void set_file_path(const char *file_path_name) {
  if (file_path) {
    file_path = file_path_name;
  }
}

/* Initialize the software rand64 implementation.  */
void software_rand64_init(void) {
  if (urandstream) {
    fclose(urandstream);
  }
  urandstream = fopen(file_path, "r");
  if (!urandstream){
    fprintf(stderr, "Cannot open file '%s'", file_path);
    abort();
  }
}

/* Return a random value, using software operations.  */
unsigned long long software_rand64(void) {
  unsigned long long int x;

  if (!urandstream) { 
        fprintf(stderr, "Error: The source is not initialized.\n");
        abort();
    }

  if (fread(&x, sizeof x, 1, urandstream) != 1){
    fprintf(stderr, "Error reading from file: %s", file_path);
    abort();
  }
  return x;
}

/* Finalize the software rand64 implementation.  */
void software_rand64_fini(void){
  fclose(urandstream);
  urandstream = NULL; 
}

/* Initialize the mrand48_r implementation with a time-based seed.
   Uses reentrant programming style that's safe for concurrent use. */
void mrand48_r_init(void){
  srand48_r(time(NULL), &state);
}

/* Return a random 64-bit value using mrand48_r.
   This uses the reentrant version of the function which is thread-safe
   by storing state in our state variable rather than global state. */
unsigned long long mrand48_r_software(void) {
  long int upper, lower;

  mrand48_r(&state, &upper);
  mrand48_r(&state, &lower);

  return ((unsigned long long)upper << 32) | (unsigned long long)lower;
}

/* Finalize the mrand48_r implementation.
   No cleanup necessary for reentrant version. */
void mrand48_r_fini(void){
  // no cleanup necessary
}