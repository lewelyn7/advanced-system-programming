obj-m := simple_module.o

KDIR  := /lib/modules/$(shell uname -r)/build
PWD   := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

test: simple_module.c
	sudo rmmod simple_module || echo "wasnt loaded"
	sudo insmod simple_module.ko
	
test2:
	cat /dev/simple