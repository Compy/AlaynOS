SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
CFLAGS = -std=c99 -Wall -O2 -nostdlib -nostartfiles

all: clean 
	+$(MAKE) -C src

clean:
	rm kernel8.elf *.o kernel8.img >/dev/null 2>/dev/null || true
	+$(MAKE) -C src clean

run:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -serial null -serial stdio

headless: all
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -nographic -monitor null -serial null -serial stdio