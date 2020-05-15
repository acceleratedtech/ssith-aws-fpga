#pragma once
#include <stdint.h>

class AWSP2;
uint64_t loadElf(AWSP2 *fpga, const char *elf_filename, size_t max_mem_size);
