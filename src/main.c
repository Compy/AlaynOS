/**
 * Main entry point file for AlaynOS. This is the first point of invocation after assembly language
 * 
 * (C) 2018 - Jimmy Lipham (http://86pixels.com)
 * This file is subject to the GPLv2 software license. Please see LICENSE file for more details.
 */

#include "uart.h"
#include "lfb.h"
#include "delays.h"
#include "power.h"
#include "util.h"
#include "mbox.h"
#include "rpi.h"
#include "sprintf.h"
#include <stdint.h>
#include <string.h>

// There is no printf here, so we alias it to the UART or framebuf
#define printf uart_printf

/**
 * Main entry point. This is the first function jumped to by our assembly preamble.
 * Basically here we set up the main interface (UART and/or framebuffer), make a call
 * to some init code to load up hardware data structures, then print some welcome information.
 */
int main()
{
    //uint8_t countdown;

    // Set up serial console
    uart_init();

    // Set up frame buffer
    //lfb_init();

    // Blank the screen
    //lfb_blank_color(0x0000FF);
    printf("\033[2J");

    // Draw our logo
    //lfb_showpicture();

    // Load main data structures, set up mailbox communications with VideoCore
    rpi_init();

    printf("Welcome to AlaynOS!\n");
    printf("Board SN:        %x\n", rpi_serial());
    printf("CPU Clock Speed: %d MHz\n\n\n", rpi_freq_cpu() / 1000000);
    printf("Max CPU Clock Speed: %d MHz\n\n\n", rpi_max_cpu_freq() / 1000000);

    // Check to see if we're emulating... If so, we probably need to fake some memory addresses for peripherals
    if (rpi_serial() == 0) {
        printf("--- Board serial invalid. I assume you're emulating in QEMU or similar... ---\n\n\n");
    }

    /*

    // End of the line. Poweroff in 10 seconds...
    for (countdown = 10; countdown > 0; countdown--) {
        if (countdown < 10) {
            printf("%c[2K\r", 27);
        }
        printf("Powering off in %d second(s)", countdown);
        wait_msec(1000000);
        if (countdown == 1) printf("\n\n\n");
    }

    // Now, QEMU is a bit funky with how it handles poweroffs, so this won't actually exit the process
    // but will put the VM into a spin waiting state. On actual hardware, this will power off the device.
    power_off();
    */
    char input_buf[32];
    uint8_t input_len = 0;
    printf("SHELL> ");
    while (1) {
        char c = uart_getc();
        uart_printf("%c", c);
        if (c == '\n') {
            if (strncmp(input_buf, "help", 32) == 0) {
                printf("Help is on the way!\n");
            } else if (strncmp(input_buf, "poweroff", 32) == 0) {
                wait_msec(100000);
                power_off();
            } else if (strncmp(input_buf, "reboot", 32) == 0) {
                wait_msec(100000);
                reset();
            } else {
                uart_printf("Unknown command %s\n", input_buf);
            }
            uart_printf("\nSHELL> ");
            input_len = 0;
            str_clear(input_buf, 32);
        } else if (input_len < 32) {
            input_buf[input_len++] = c;
        }
    }
    return 0;
}