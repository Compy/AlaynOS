#include "gpio.h"
#include "mbox.h"
#include <stdint.h>

/* Mailbox message buffer */
volatile unsigned int __attribute__((aligned(16))) mbox[36];

/**
 * Make a mailbox call, returns 0 on failure, non-zero on success.
 * Sends a call and waits for a response from the GPU
 */
int mbox_call(unsigned char ch) {
    unsigned int r = (((unsigned int)((unsigned long) &mbox) & ~0xF) | (ch & 0xF));
    // Wait until we can write to the mailbox
    do { asm volatile("nop"); } while (*MBOX_STATUS & MBOX_FULL);

    // Write the maddr of our message to the mailbox with channel identifier
    *MBOX_WRITE = r;

    // Wait for the response
    while (1) {
        do { asm volatile("nop"); } while (*MBOX_STATUS & MBOX_EMPTY);

        // Check if its actually a response to our message
        if (r == *MBOX_READ) {
            // Is it a valid successful response?
            return mbox[1] == MBOX_RESPONSE;
        }
    }
    return 0;
}

void mailbox_write(MAILBOX_CHANNEL chan, uint32_t msg) {
    // Wait until we can write
    while ((*MBOX1_STATUS & MBOX_FULL) != 0) {

    }
    *MBOX1_FIFO = (msg << 4) | chan;
}

uint32_t mailbox_read(MAILBOX_CHANNEL chan) {
    uint32_t data;
    do {
        while (*MBOX_STATUS & MBOX_EMPTY) {

        }
    } while (((data = *MBOX_READ) & 0xfU) != chan);
    return data >> 4;
}