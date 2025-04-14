CC := gcc
CFLAGS := -Wall -Wextra -nostdlib -Os -I../lib
# Prevents "undefined reference to `__stack_chk_fail'"
CFLAGS+=-fno-stack-protector
CFLAGS+=-ffreestanding
CFLAGS+=-nostartfiles
CFLAGS+=-Wl,--build-id=none -Wa,--noexecstack
CFLAGS+=-mno-shstk -fcf-protection=none
CFLAGS+=-g0 -fno-ident -fno-asynchronous-unwind-tables -fno-unwind-tables
LDFLAGS := -L ../lib -T comfile.x

#TARGETS := cat.com echo.com env.com hello.com factor.com tac.com tiny.com tinier.com
CSRCS:=$(wildcard *.c)
ASRCS:=$(wildcard *.asm)
TARGETS:=$(patsubst %.c,%.com,$(CSRCS))
TARGETS+=$(patsubst %.asm,%.com,$(ASRCS))

.PHONY: all clean

all: $(TARGETS)

#.PRECIOUS:%.o
%.com: %.o
#	strip --strip-all $^
	ld $(LDFLAGS) -o $@ $^ -lcom

%.com: %.asm
	nasm -f bin -o $@ $^
	chmod +x $@

clean:
	rm -f $(TARGETS) $(TARGETS:.com=.o) *.com
