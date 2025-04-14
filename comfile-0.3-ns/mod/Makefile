obj-m = comfile.o
kver = $(shell uname -r)
all:
	make -C /lib/modules/$(kver)/build/ M=$(PWD) modules
clean:
	make -C /lib/modules/$(kver)/build M=$(PWD) clean
