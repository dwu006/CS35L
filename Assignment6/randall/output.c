#include "output.h"

bool writebytes(unsigned long long x, int nbytes) {
  do {
    if (putchar(x) < 0)
      return false;
    x >>= CHAR_BIT;
    nbytes--;
  } while (0 < nbytes);

  return true;
}

int handle_output(char *input, char *output, int nbytes) {
  void (*initialize)(void);
  unsigned long long (*rand64)(void);
  void (*finalize)(void);

  // Error handling for missing input or output

   // Handle input options
   if (strcmp(input, "rdrand") == 0) {
     // Handles hardware case
     initialize = hardware_rand64_init;
     rand64 = hardware_rand64;
     finalize = hardware_rand64_fini;
   } else if (strcmp(input, "mrand48_r") == 0) {
      initialize = mrand48_r_init;
      rand64 = mrand48_r_software;
      finalize = mrand48_r_fini;
   } else if (strncmp(input, "/", 1) == 0) {
      set_file_path(input);
      initialize = software_rand64_init;
      rand64 = software_rand64;
      finalize = software_rand64_fini;
   } else {
      fprintf(stderr, "Invalid input: %s\n", input);
      return EINVAL; // invalid argument
   }


  initialize();

  int wordsize = sizeof(rand64());
  int output_errno = 0;

  if (strcmp(output, "stdio") == 0) {
      // Default stdio option
      do {
        unsigned long long x = rand64();
        int outbytes = nbytes < wordsize ? nbytes : wordsize;
        if (!writebytes(x, outbytes)) {
          output_errno = errno;
          break;
        }
        nbytes -= outbytes;
      } while (0 < nbytes);

    if (fclose(stdout) != 0)
      output_errno = errno;

    if (output_errno) {
      errno = output_errno;
      perror("output");
      return output_errno;
    }
  } else {
    // Handle -o N option
    char *endptr;
    long num = strtoll(output, &endptr, 10);

    if (*endptr != '\0' || num <= 0) {
      fprintf(stderr, "Not a valid -o N value: %s\n", output);
      return EINVAL;
    }

    unsigned char *buffer = (unsigned char *)malloc(num);
    if (!buffer) {
      perror("malloc failed");
      return ENOMEM;
    }

    unsigned long long x;
    int bytes_left = 0;

    do {
      int bytes_to_write = (nbytes < num) ? nbytes : num;
      for (int i = 0; i < bytes_to_write; i ++) {
        if (bytes_left == 0) {
          x = rand64();
          bytes_left = sizeof(x);
        }

        buffer[i] = (unsigned char)(x & 0xFF);                       
        x >>= 8;                     
        bytes_left--; 
      }

      ssize_t bytes_written = write(STDOUT_FILENO, buffer, bytes_to_write);
      if (bytes_written <= 0) {
        perror("write failed");
        free(buffer);  
        return errno;  
      }
      nbytes -= bytes_to_write;
    } while (nbytes > 0);
    
    free(buffer);
  }
  finalize();

  return 0;
}