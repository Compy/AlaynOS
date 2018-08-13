# AlaynOS
### Embedded Operating System for the Raspberry Pi v3


## About AlaynOS
AlaynOS is an embedded environment for the Raspberry Pi 3. I hesitate to call it an actual Operating System because it is only designed to run one task per core, so really it is an embedded development environment.

There is no MMU, no fancy graphics routines (yet), and no multicore support (yet). 
Here is the roadmap:

- [ ] Multicore (SMP) support
- [ ] Console input support (USB keyboards)
- [ ] VideoCore support for HD video
- [ ] SD card filesystem support (FAT32 and/or custom FS)
- [ ] IPC between core tasks
- [ ] Native python scripting support via MicroPython

Here is what has been completed:
- [x] Framebuffer support
- [x] UART support
- [x] QEMU support for development
- [x] Mailbox support with the VideoCore hardware

## Why?
I believe Operating Systems now are fairly bloated. The barrier to embedded development is very high, and often times new developers in the space attempt to wrap their applications inside of Raspbian or other full-scale operating systems. With the uptick of IoT, I believe that security and ease of development can be served by taking a minimalistic approach to embedded development such that the ONLY process running on the hardware is the primary process that the device serves.
