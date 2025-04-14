CC := gcc
CFLAGS := -Wall -Wextra -nostdlib -Os

OBJS := errno.o write.o read.o open.o close.o exit.o brk.o strlen.o fprint.o

.PHONY: all clean

all: libcom.a startup.o

libcom.a: $(OBJS) libcom.h
	$(AR) r $@ $(OBJS)

startup.o: startup.asm
	nasm -f elf64 -o $@ $^

clean:
	rm -f $(OBJS) libcom.a startup.o
