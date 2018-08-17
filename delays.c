/**
 * Implements cycle-accurate delays for the rest of the system. We use both software level
 * delays and hardware level delays with SYSTMR.
 * 
 * (C) 2018 - Jimmy Lipham (http://86pixels.com)
 * This file is subject to the GPLv2 software license. Please see LICENSE file for more details.
 */

#include "gpio.h"

#define SYSTMR_LO       ((volatile unsigned int*)(MMIO_BASE + 0x00003004))
#define SYSTMR_HI       ((volatile unsigned int*)(MMIO_BASE + 0x00003008))

/**
 * Wait N CPU cycles (ARM CPU only)
 */
void wait_cycles(unsigned int n) {
    if (n) {
        while (n--) { asm volatile("nop"); }
    }
}

/**
 * Wait N microsec (ARM CPU only)
 */
void wait_msec(unsigned int n) {
    register unsigned long f, t, r;
    // Get the current counter frequency
    asm volatile ("mrs %0, cntfrq_el0" : "=r"(f));
    // Read the current counter
    asm volatile ("mrs %0, cntpct_el0" : "=r"(t));
    // calculate expire value for counter
    t += ((f/1000)*n)/1000;
    do { asm volatile ("mrs %0, cntpct_el0" : "=r"(r)); } while (r < t);
}

/**
 * Get system timer's counter
 */
unsigned long get_system_timer() {
    unsigned int h = -1, l;
    // We must read MMIO area as two separate 32 bit reads
    h = *SYSTMR_HI;
    l = *SYSTMR_LO;

    // If the high word changed during the read, we need to repeat it
    if (h != *SYSTMR_HI) {
        h = *SYSTMR_HI;
        l = *SYSTMR_LO;
    }

    // Compose long int value
    return ((unsigned long) h << 32) | l;
}

/**
 * Wait N microseconds (with BCM system timer)
 */
void wait_msec_st(unsigned int n) {
    unsigned long t = get_system_timer();
    // We must check if its non zero, because qemu does not emulate the system timer
    // and returning a constant zero would cause infinite looping
    if (t) {
        while (get_system_timer() < t+n);
    }
}