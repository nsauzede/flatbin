obj-m = comfile.o
kver = $(shell uname -r)
all:
	make -C /lib/modules/$(kver)/build M=$(PWD) modules
	-sudo rmmod -f comfile
	sudo insmod comfile.ko
clean:
	make -C /lib/modules/$(kver)/build M=$(PWD) clean
