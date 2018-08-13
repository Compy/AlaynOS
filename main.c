#include "uart.h"
#include "lfb.h"
#include "delays.h"
#include "power.h"
#include "util.h"
#include "mbox.h"
#include "rpi.h"
#include "sprintf.h"
#include <stdint.h>

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

    /*
    uart_puts("Clock speed: ");
    uart_puts(itoa(rpi_freq_cpu(), buf, 10));
    uart_puts(" (CPU) ");
    uart_puts(itoa(rpi_freq_core(), buf, 10));
    uart_puts(" (CORE)\n");
    */

    printf("Hello world %x\n", rpi_serial());


    lfb_print(0, line, COLOR_GRAY, "Board SN: ");
    lfb_print(10, line, COLOR_WHITE, dec_to_hex64(rpi_serial(), buf));

    lfb_print(0, ++line, COLOR_GRAY, "CPU Clock Speed: ");
    lfb_print(17, line, COLOR_WHITE, itoa(rpi_freq_cpu() / 1000000, buf, 10));
    lfb_print(21, line, COLOR_WHITE, "MHz");

    lfb_print(0, ++line, COLOR_CYAN, "Welcome to AlaynOS!");

    wait_msec(2000000);

    for (countdown = 100; countdown > 0; countdown--) {
        lfb_print(13, ++line, 0xFF0000, "  ");
        lfb_print(0, line, COLOR_WHITE, "Resetting in ");
        lfb_print(13, line, COLOR_WHITE, itoa(countdown, buf, 10));
        lfb_print(16, line, COLOR_WHITE, "seconds...");
        wait_msec(1000000);
    }
    reset();
    while (1) {
        uart_puts(uart_getc());
    }
}