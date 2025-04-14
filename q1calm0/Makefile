ext:=calm
obj-m = $(ext)file.o
kver = $(shell uname -r)
ccflags-y += -DMY_EXT=\"$(ext)\"

ASMS:=$(wildcard *.asm)
BINS:=$(patsubst %.asm,%.$(ext),$(ASMS))
all: $(BINS) $(ext)file.ko

%.ko: %.c
	$(MAKE) -C /lib/modules/$(kver)/build M=$(PWD) modules
	-sudo rmmod -f $(ext)file
	sudo insmod $(ext)file.ko

%.$(ext): %.asm
	nasm -f bin -o $@ $^
	chmod +x $@

clean:
	$(MAKE) -C /lib/modules/$(kver)/build M=$(PWD) clean
	$(RM) *.$(ext)
