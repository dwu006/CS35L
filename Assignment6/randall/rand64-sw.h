#ifndef RAND64_SW_H
#define RAND64_SW_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

/* Software implementation.  */
void set_file_path(const char *file_path_name);
void software_rand64_init(void);
unsigned long long software_rand64(void);
void software_rand64_fini(void);
void mrand48_r_init(void);
unsigned long long mrand48_r_software(void);
void mrand48_r_fini(void);

#endif 