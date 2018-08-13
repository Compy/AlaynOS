#include "uart.h"
#include "mbox.h"
#include "homer.h"

typedef struct {
    unsigned int magic;
    unsigned int version;
    unsigned int headersize;
    unsigned int flags;
    unsigned int numglyph;
    unsigned int bytesperglyph;
    unsigned int height;
    unsigned int width;
    unsigned char glyphs;
} __attribute__((packed)) psf_t;
extern volatile unsigned char _binary_font_psf_start;
extern volatile unsigned char _end;

unsigned int width, height, pitch;
unsigned char *lfb;

/**
 * Set screen resoltuion to 1024 x 768
 */
void lfb_init() {
    mbox[0] = 35*4;             // msg len
    mbox[1] = MBOX_REQUEST;

    mbox[2] = 0x48003;          // set physical width/height
    mbox[3] = 8;
    mbox[4] = 8;
    mbox[5] = 1024;
    mbox[6] = 768;

    mbox[7] = 0x48004;          // set virtual width/height
    mbox[8] = 8;
    mbox[9] = 8;
    mbox[10] = 1024;
    mbox[11] = 768;

    mbox[12] = 0x48009;         // set virtual offsets
    mbox[13] = 8;
    mbox[14] = 8;
    mbox[15] = 0;
    mbox[16] = 0;

    mbox[17] = 0x48005;         // set depth
    mbox[18] = 4;
    mbox[19] = 4;
    mbox[20] = 32;              // FrameBufferInfo.depth

    mbox[21] = 0x48006;         // Set pixel order
    mbox[22] = 4;
    mbox[23] = 4;
    mbox[24] = 2;               // RGB
    
    mbox[25] = 0x40001;         // Get framebuffer, gets alignment on request
    mbox[26] = 8;
    mbox[27] = 8;
    mbox[28] = 4096;            // FrameBufferInfo.pointer
    mbox[29] = 0;               // FrameBufferInfo.size

    mbox[30] = 0x40008;         // Get pitch
    mbox[31] = 4;
    mbox[32] = 4;
    mbox[33] = 0;               // FrameBufferInfo.pitch

    mbox[34] = MBOX_TAG_LAST;

    if (mbox_call(MBOX_CH_PROP) && mbox[20] == 32 && mbox[28] != 0) {
        mbox[28] &= 0x3FFFFFFF;
        width = mbox[5];
        height = mbox[6];
        pitch = mbox[33];
        lfb = (void*)((unsigned long)mbox[28]);
    } else {
        uart_puts("Unable to set screen resolution to 1024 x 768 x 32\n");
    }
}

/**
 * Show a picture
 */
void lfb_showpicture() {
    int x,y;
    unsigned char *ptr = lfb;
    char *data = homer_data, pixel[4];

    //ptr += (height - homer_height) / 2 * pitch + (width - homer_width) * 2;
    ptr += pitch + (width - homer_width - 8) * 4;

    for (y = 0; y < homer_height; y++) {
        for (x = 0; x < homer_width; x++) {
            HEADER_PIXEL(data, pixel);
            *((unsigned int *)ptr) =* ((unsigned int *)&pixel);
            ptr += 4;
        }
        ptr += pitch - homer_width * 4;
    }
}

void lfb_blank_color(unsigned int color) {
    unsigned char *ptr = lfb;
    int x,y;

    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            *((unsigned int *)ptr) = color;
            ptr += 4;
        }
        ptr += pitch - width * 4;
    }
}

void lfb_blank() {
    lfb_blank_color(0);
}

/**
 * Display a string
 */
void lfb_print(int x, int y, unsigned int color, char *s) {
    // Get our font
    psf_t *font = (psf_t*)&_binary_font_psf_start;

    // Draw the next character if its not zero
    while (*s) {
        // Get the offset of the glyph.
        unsigned char *glyph = (unsigned char*)&_binary_font_psf_start + font->headersize +
            (*((unsigned char*)s) < font->numglyph ? *s : 0) * font->bytesperglyph;
        
        // Calculate the offset on screen
        int offs = (y * font->height * pitch) + (x * (font->width + 1) * 4);

        // Variables
        int i, j, line, mask, bytesperline = (font->width + 7) / 8;

        // Handle carriage return
        if (*s == '\r') {
            x = 0;
        } else if (*s == '\n') {
            x = 0;
            y++;
        } else {
            // Display a character
            for (j = 0; j < font->height; j++) {
                // Display one row
                line = offs;
                mask = 1 << (font->width - 1);
                for (i = 0; i < font->width; i++) {
                    // If a bit is set, we use white color, otherwise black
                    if (((int)*glyph) & mask) {
                        *((unsigned int*)(lfb + line)) = color;
                    }
                    mask >>= 1;
                    line += 4;
                }
                // Adjust to next line
                glyph += bytesperline;
                offs += pitch;
            }
            x++;
        }
        // Next character
        s++;
    }
}

void lfb_blankline(int line) {
    // Get our font
    lfb_print(0, line, 0x0, "                                                          ");
}

void lfb_testpattern() {
    unsigned char *ptr = lfb;
    int x,y;

    for (y = 0; y < height/2; y++) {
        for (x = 0; x < width; x++) {
            if (x % 2 == 0) {
                *((unsigned int *)ptr) = 0xFF0000;
            } else {
                *((unsigned int *)ptr) = 0;
            }
            ptr += 4;
        }
        ptr += pitch - width * 4;
    }
}

/**
 * Display a string
 */
void printf(char *fmt, ...) {
    __builtin_va_list args;
    __builtin_va_start(args, fmt);
    // we don't have memory allocation yet, so we
    // simply place our string after our code
    char *s = (char*)&_end;
    // use sprintf to format our string
    vsprintf(s,fmt,args);
    // print out as usual
    lfb_print(0, 16, 0xFFFFFF, s);
}