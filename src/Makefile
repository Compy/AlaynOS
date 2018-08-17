SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
INC += -I.
INC += -I../include
CFLAGS = $(INC) -Wall -O2 -ffreestanding -nostdlib -nostartfiles

all: clean kernel8.img

start.o: start.S
	aarch64-none-elf-gcc $(CFLAGS) -c start.S -o start.o

%.o: %.c
	aarch64-none-elf-gcc $(CFLAGS) -c $< -o $@

font.o: font.psf
	aarch64-none-elf-ld -r -b binary -o font.o font.psf

kernel8.img: start.o font.o $(OBJS)
	aarch64-none-elf-ld -nostdlib -nostartfiles start.o font.o $(OBJS) -T link.ld -o kernel8.elf
	aarch64-none-elf-objcopy -O binary kernel8.elf ../kernel8.img

clean:
	rm kernel8.elf *.o kernel8.img >/dev/null 2>/dev/null || true