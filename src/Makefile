SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
INC += -I.
INC += -I../include
INC += -I/usr/local/Caskroom/gcc-arm-embedded/7-2018-q2-update/gcc-arm-none-eabi-7-2018-q2-update/arm-none-eabi/include
CFLAGS = $(INC) -std=c99 -Wall -O2 -nostdlib -nostartfiles

CROSS_COMPILE = aarch64-linux-gnu

all: clean kernel8.img

start.o: start.S
	$(CROSS_COMPILE)-gcc $(CFLAGS) -c start.S -o start.o

%.o: %.c
	$(CROSS_COMPILE)-gcc $(CFLAGS) -c $< -o $@

font.o: font.psf
	$(CROSS_COMPILE)-ld -r -b binary -o font.o font.psf

kernel8.img: start.o font.o $(OBJS)
	$(CROSS_COMPILE)-ld -nostdlib -nostartfiles start.o font.o $(OBJS) -T link.ld -o kernel8.elf
	$(CROSS_COMPILE)-objcopy -O binary kernel8.elf ../kernel8.img

clean:
	rm kernel8.elf *.o kernel8.img >/dev/null 2>/dev/null || true
