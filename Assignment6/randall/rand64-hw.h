#ifndef RAND64_HW_H
#define RAND64_HW_H

#include <cpuid.h>
#include <immintrin.h>
#include <stdbool.h>

struct cpuid;
struct cpuid cpuid(unsigned int leaf, unsigned int subleaf);
_Bool rdrand_supported(void);
void hardware_rand64_init(void);
unsigned long long hardware_rand64(void);
void hardware_rand64_fini(void);

#endif 