obj-m := advanced.o

KDIR  := /lib/modules/$(shell uname -r)/build
PWD   := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

test:
	make default
	sudo rmmod zad2
	sudo insmod zad2.ko

