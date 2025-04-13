obj-m = hello.o
kver = $(shell uname -r)
all:
	$(MAKE) -C /lib/modules/$(kver)/build M=$(PWD) modules

clean:
	$(MAKE) -C /lib/modules/$(kver)/build M=$(PWD) clean
