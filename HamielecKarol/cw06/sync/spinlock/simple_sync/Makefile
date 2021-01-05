obj-m := simple_module.o

KDIR  := /lib/modules/$(shell uname -r)/build
PWD   := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

test:
	sudo rmmod simple_module.ko
	sudo insmod simple_module
	
test2:
	cat /dev/simple