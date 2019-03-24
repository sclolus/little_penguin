module_name = "fortytwo_dev"
src-m += fortytwo_dev.c
obj-m += $(src-m:.c=.o)
module-obj = $(obj-m:.o=.ko)
KERNELRELEASE=$(shell uname -r)
KDIR=/lib/modules/$(shell uname -r)/build
EXTRA_FLAGS= -Wall -Wextra -Werror #-Weverything damn the Weverything flag not in gcc

TEST_FILE = "test_bin"
src-test = test.c
obj-test = $(src-test:.c=.o)

all:
	make -C $(KDIR) M=$(PWD) modules

modules_install: all
	sudo make -C $(KDIR) M=$(PWD) modules_install
	sudo cp $(module-obj) /lib/modules/$(shell uname -r)/extra
	sudo depmod -a

clean:
	make -C $(KDIR) M=$(PWD) clean
	rm -f $(TEST_FILE) $(obj-test)

test: all $(TEST_FILE)
	-sudo rmmod -f $(module_name)
	sudo insmod $(module_name).ko
	-./$(TEST_FILE) || echo "Test binary exited with failure exit code"
	sudo rmmod -f $(module_name)

$(TEST_FILE): $(obj-test)
	$(CC) $(EXTRA_FLAGS) $^ -o $(TEST_FILE)
