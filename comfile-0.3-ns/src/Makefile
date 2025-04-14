CC := gcc
CFLAGS := -Wall -Wextra -nostdlib -Os -I../lib
LDFLAGS := -L ../lib -T comfile.x

TARGETS := cat.com echo.com env.com hello.com factor.com tac.com

.PHONY: all clean

all: $(TARGETS)

%.com: %.o
	ld $(LDFLAGS) -o $@ $^ -lcom

clean:
	rm -f $(TARGETS) $(TARGETS:.com=.o)
