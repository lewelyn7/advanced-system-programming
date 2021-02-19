obj-m := linked_buffer.o

KDIR  := /lib/modules/$(shell uname -r)/build
PWD   := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules


test: linked_buffer.c
	sudo rmmod linked_buffer || echo "wasnt loaded"
	sudo insmod linked_buffer.ko
	
test2:
	echo "w szczebrzeszynie chrzaszcz brzmi w trzcinie" > /dev/linked