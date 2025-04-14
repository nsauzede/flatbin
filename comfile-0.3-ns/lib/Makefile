CC := gcc
CFLAGS := -Wall -Wextra -nostdlib -Os
# Prevents "undefined reference to `__stack_chk_fail'"
CFLAGS+=-fno-stack-protector
CFLAGS+=-ffreestanding
CFLAGS+=-nostartfiles
CFLAGS+=-Wl,--build-id=none -Wa,--noexecstack
CFLAGS+=-mno-shstk -fcf-protection=none
CFLAGS+=-g0 -fno-ident -fno-asynchronous-unwind-tables -fno-unwind-tables

OBJS := errno.o write.o read.o open.o close.o exit.o brk.o strlen.o fprint.o

.PHONY: all clean

all: libcom.a startup.o

libcom.a: $(OBJS) libcom.h
	$(AR) r $@ $(OBJS)

startup.o: startup.asm
	nasm -f elf64 -o $@ $^

clean:
	rm -f $(OBJS) libcom.a startup.o
