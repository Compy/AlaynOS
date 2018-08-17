#include "uart.h"
#include "lfb.h"
#include "delays.h"
#include "power.h"
#include "util.h"
#include "mbox.h"
#include "rpi.h"
#include "sprintf.h"
#include <stdint.h>

#define printf uart_printf

void main()
{
    uint8_t countdown;
    char buf[16];
    uint8_t line;

    // set up serial console
    uart_init();
    lfb_init();

    lfb_blank_color(0x0000FF);

    lfb_showpicture();

    rpi_init();
    // CLEAR SCREEN
    printf("\033[2J");
    ////////////////
    printf("Welcome to AlaynOS!\n");
    printf("Board SN: %x\n", rpi_serial());
    printf("CPU Clock Speed: %d MHz\n", rpi_freq_cpu() / 1000000);

    for (countdown = 10; countdown > 0; countdown--) {
        if (countdown < 10) {
            printf("%c[2K\r", 27);
        }
        printf("Resetting in %d second(s)", countdown);
        wait_msec(1000000);
    }
    reset();
    while (1) {
        uart_puts(uart_getc());
    }
}