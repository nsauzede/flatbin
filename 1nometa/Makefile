obj-m = comfile.o
kver = $(shell uname -r)

all: a.com b.com comfile.ko

%.ko: %.c
	$(MAKE) -C /lib/modules/$(kver)/build M=$(PWD) modules

%.com: %.asm
	nasm -f bin -o $@ $^
	chmod +x $@

clean:
	$(MAKE) -C /lib/modules/$(kver)/build M=$(PWD) clean
	$(RM) *.com
