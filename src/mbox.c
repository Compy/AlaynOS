/**
 * Mailbox handling functions for the Raspberry Pi (Broadcom) hardware. This allows us to query and control
 * certain device peripherals on the board.
 * 
 * (C) 2018 - Jimmy Lipham (http://86pixels.com)
 * This file is subject to the GPLv2 software license. Please see LICENSE file for more details.
 */

#include "gpio.h"
#include "mbox.h"
#include <stdint.h>

// Mailbox message buffer. Make sure everything is on a 16 byte boundary, or
// bad shit tends to happen in a fixed memory environment
volatile unsigned int __attribute__((aligned(16))) mbox[36];

/**
 * Make a mailbox call, returns 0 on failure, non-zero on success.
 * Sends a call and waits for a response from the GPU
 */
int mbox_call(unsigned char ch) {
    // Set the appropriate bit given the channel specified
    unsigned int r = (((unsigned int)((unsigned long) &mbox) & ~0xF) | (ch & 0xF));
    // Wait until we can write to the mailbox
    do { __asm__ volatile("nop"); } while (*MBOX_STATUS & MBOX_FULL);

    // Write the maddr of our message to the mailbox with channel identifier
    *MBOX_WRITE = r;

    // Spin wait for the response
    while (1) {
        do { __asm__ volatile("nop"); } while (*MBOX_STATUS & MBOX_EMPTY);

        // Check if its actually a response to our message
        if (r == *MBOX_READ) {
            // Is it a valid successful response?
            return mbox[1] == MBOX_RESPONSE;
        }
    }
    return 0;
}

/**
 * Send the given message to the given mailbox.
 * Unlike other implementations, we do not wait for a reply here.
 */
void mailbox_write(MAILBOX_CHANNEL chan, uint32_t msg) {
    // Wait until we can write
    do { __asm__ volatile("nop"); } while (*MBOX1_STATUS & MBOX_FULL);

    // Write the message and the given channel to the input section of memory
    *MBOX1_FIFO = (msg << 4) | chan;
}

/**
 * Read the given channel for a mailbox response message.
 */
uint32_t mailbox_read(MAILBOX_CHANNEL chan) {
    uint32_t data;
    do {
        while (*MBOX_STATUS & MBOX_EMPTY) {

        }
    } while (((data = *MBOX_READ) & 0xfU) != chan);
    return data >> 4;
}