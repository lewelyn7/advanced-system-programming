obj-m := circular.o

KDIR  := /lib/modules/$(shell uname -r)/build
PWD   := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

test:
	./cr-dev.sh
	echo "bardzo dlugi tekst na ponad 40 znakow zrobiony chyba juz bylo" > /dev/circular
	cat /dev/circular