/**
 * Hardware specific information functions for the Raspberry Pi 3.
 * 
 * (C) 2018 - Jimmy Lipham (http://86pixels.com)
 * This file is subject to the GPLv2 software license. Please see LICENSE file for more details.
 */

#include <stdint.h>
#include "rpi.h"
#include "mbox.h"

static uint32_t freq_cpu  = 0;
static uint32_t freq_core = 0;
static uint64_t serial = 0;

uint32_t rpi_freq_core() {
    return freq_core;
}

uint32_t rpi_freq_cpu() {
    return freq_cpu;
}

uint64_t rpi_serial() {
    return serial;
}

uint8_t is_emulating() {
    return (rpi_serial() == 0) ? 1 : 0;
}

enum MB_BUFF { BUFSIZE=0, MB_REQ_RES, \
               TAG0, VALBUFSIZE0, REQ_RES0, VALUE00, VALUE01,\
               TAG1, VALBUFSIZE1, REQ_RES1, VALUE10, VALUE11,\
               ENDTAG };

static void get_serial() {
    // Get the board's serial number
    mbox[0] = 8*4;                  // msg len
    mbox[1] = MBOX_REQUEST;         // msg type
    mbox[2] = MBOX_TAG_GETSERIAL;   // request type
    mbox[3] = 8;                    // buffer size
    mbox[4] = 8;                    // response 0
    mbox[5] = 0;                    // clear output buf (value 00)
    mbox[6] = 0;                    // value 01
    mbox[7] = MBOX_TAG_LAST;        // magic end tag
    if (mbox_call(MBOX_CH_PROP)) {
        serial = ((uint64_t)mbox[6]) << 32 | ((uint64_t)mbox[5]);
    }
}

static void get_clock_value() {
    mbox[0] = 13*4;                  // msg len
    mbox[1] = MBOX_REQUEST;         // msg type
    mbox[2] = MB_TAG_GET_CLOCK_RATE;   // request type
    mbox[3] = 8;                    // buffer size
    mbox[4] = 0;                    // response 0
    mbox[5] = CLKID_ARM;            // value 00
    mbox[6] = 0;                    // value 01
    mbox[7] = MB_TAG_GET_CLOCK_RATE; // request type 2
    mbox[8] = 8;                    //buf size 2
    mbox[9] = 0;                    // response 1
    mbox[10] = CLKID_CORE;          // second value 0
    mbox[11] = 0;                   // second value 1
    mbox[12] = MBOX_TAG_LAST;        // magic end tag

    if (mbox_call(MBOX_CH_PROP)) {
        freq_cpu = mbox[6];
        freq_core = mbox[11];
    }
}

void rpi_init() {
    get_clock_value();
    get_serial();
// any initialization 
}