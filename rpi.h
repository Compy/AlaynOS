#ifndef __INCLUDE_RPI_H
#define __INCLUDE_RPI_H

#include <stdint.h>

#define CLKID_ARM   3
#define CLKID_CORE  4

void rpi_init();

uint32_t rpi_freq_core();
uint32_t rpi_freq_cpu();
uint64_t rpi_serial();
uint8_t is_emulating();

#endif // __INCLUDE_RPI_H