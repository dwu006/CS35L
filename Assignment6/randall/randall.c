/* Generate N bytes of random output.  */

/* When generating output this program uses the x86-64 RDRAND
   instruction if available to generate random numbers, falling back
   on /dev/random and stdio otherwise.

   This program is not portable.  Compile it with gcc -mrdrnd for a
   x86-64 machine.

   Copyright 2015, 2017, 2020 Paul Eggert

   This program is free software: you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

// #include <limits.h>
#include "options.h"
#include "output.h"
#include "rand64-hw.h"
#include "rand64-sw.h"
#include "options.h"
#include "getopt.h"
#include <stdio.h>
#include <string.h>

/* Ensure rdrand_supported function is declared before use */
_Bool rdrand_supported(void);

/* Main program, which outputs N bytes of random data.  */
int main(int argc, char **argv) {
  char *input = "rdrand";
  char *output = "stdio";

  int opt;
  while ((opt = getopt(argc, argv, "i:o:")) != -1) {
    switch(opt) {
      case 'i':
        input = optarg;
        break;
      case 'o':
        output = optarg;
        break;
      case '?':
        fprintf(stderr, "Usage: %s [-i input] [-o output] NBYTES\n", argv[0]);
        return 1;
    }
  }

/*
  fprintf(stdout, "input: %s, output: %s\n", input, output);
  fprintf(stdout, "optind: %d, argc: %d\n", optind, argc);
*/

  if ((strcmp(input, "rdrand") == 0) && !rdrand_supported()){
    fprintf(stderr, "RDRAND is not supported on the current machine.\n");
    return 1;
  }

  /* Check arguments.  */
  long long nbytes = handle_nbytes(argc, argv);

  /* If there's no work to do, don't worry about which library to use. If an error occurred, exit early */
  if (nbytes <= 0) {
    return nbytes == 0 ? 0 : 1;
  }

  /* Now that we know we have work to do, arrange to use the
     appropriate library.  */
  int result = handle_output(input, output, nbytes);
  
  return result;
}
