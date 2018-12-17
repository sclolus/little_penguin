src-m += usb_kbd_detect.c
obj-m += $(src-m:.c=.o)
module-obj = $(obj-m:.o=.ko)
KERNELRELEASE=$(shell uname -r)
KDIR=/lib/modules/$(shell uname -r)/build

all:
	make -C $(KDIR) M=$(PWD) modules

modules_install: all
	make -C $(KDIR) M=$(PWD) modules_install
	cp $(module-obj) /lib/modules/$(shell uname -r)/extra
	cp usb-keyboard.rules /run/udev/rules.d/70-usb-keyboard.rules
	sudo depmod -a

clean:
	make -C $(KDIR) M=$(PWD) clean

test:
	udevadm control --reload-rules
