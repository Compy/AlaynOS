/**
 * Power handling functions for the entire system. This communicates with the raspberry pi in order to set
 * hardware peripherals into a low power state (off) or perform jumps as needed for system resets by way
 * of the watchdog timer.
 * 
 * (C) 2018 - Jimmy Lipham (http://86pixels.com)
 * This file is subject to the GPLv2 software license. Please see LICENSE file for more details.
 */

#include "gpio.h"
#include "mbox.h"
#include "delays.h"
#include "uart.h"

#define PM_RSTC         ((volatile unsigned int*)(MMIO_BASE+0x0010001c))
#define PM_RSTS         ((volatile unsigned int*)(MMIO_BASE+0x00100020))
#define PM_WDOG         ((volatile unsigned int*)(MMIO_BASE+0x00100024))
#define PM_WDOG_MAGIC   0x5a000000
#define PM_RSTC_FULLRST 0x00000020

/**
 * Shutdown the board
 */
void power_off() {
    unsigned long r;

    uart_puts("*** SYSTEM POWER OFF ***\n");

    // Power off devices one by one
    for (r = 0; r < 16; r++) {
        mbox[0] = 8*4;
        mbox[1] = MBOX_REQUEST;
        mbox[2] = MBOX_TAG_SETPOWER;
        mbox[3] = 8;
        mbox[4] = 8;
        mbox[5] = (unsigned int)r;
        mbox[6] = 0;
        mbox[7] = MBOX_TAG_LAST;
        mbox_call(MBOX_CH_PROP);
    }

    // power off GPIO pins (but skip VCC pins)
    *GPFSEL0 = 0;
    *GPFSEL1 = 0;
    *GPFSEL2 = 0;
    *GPFSEL3 = 0;
    *GPFSEL4 = 0;
    *GPFSEL5 = 0;
    *GPPUD = 0;
    wait_cycles(150);
    *GPPUDCLK0 = 0xffffffff;
    *GPPUDCLK1 = 0xffffffff;
    wait_cycles(150);
    *GPPUDCLK0 = 0;
    *GPPUDCLK1 = 0;

    // Power of the SoC (GPU + CPU)
    r = *PM_RSTS;
    r &= ~0xfffffaaa;
    r |= 0x555; // Partition 63 used to indicate halt

    *PM_RSTS = PM_WDOG_MAGIC | r;
    *PM_WDOG = PM_WDOG_MAGIC | 10;
    *PM_RSTC = PM_WDOG_MAGIC | PM_RSTC_FULLRST;
}

/**
 * Reboot
 */
void reset() {
    unsigned int r;

    uart_puts("*** SYSTEM RESET ***\n");
    
    // Trigger a restart by instructing the GPU to boot from partition 0
    r = *PM_RSTS;
    r &= ~0xfffffaaa;
    *PM_RSTS = PM_WDOG_MAGIC | r; // Boot from partition 0
    *PM_WDOG = PM_WDOG_MAGIC | 10;
    *PM_RSTC = PM_WDOG_MAGIC | PM_RSTC_FULLRST;
}