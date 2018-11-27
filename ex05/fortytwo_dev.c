// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
/* #include <linux/delay.h> */
/* #include <linux/kobject.h> */
#include <linux/device.h>
#include <linux/usb.h>
#include <linux/mod_devicetable.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <linux/errno.h>

MODULE_ALIAS("fortytwo_miscdev");
MODULE_LICENSE("GPL v2");

#define DEVICE_NAME "fortytwo"
#define LOGIN "sclolus"

static ssize_t fortytwo_dev_read(struct file *file, char __user *to, size_t size, loff_t *_offset)
{
	uint64_t ret;

	ret = simple_read_from_buffer(to, size, _offset, LOGIN, sizeof(LOGIN) - 1);
	/* printk(KERN_INFO "simple_read_from_buffer: %lld", ret); */
	return (ret);
}

static ssize_t fortytwo_dev_write(struct file *file, const char __user *from, size_t size, loff_t *_offset)
{
	static char buffer[sizeof(LOGIN)];
	uint64_t ret = 0;

	ret = simple_write_to_buffer(buffer, sizeof(LOGIN) - 1, _offset, from, size);
	buffer[ret] = 0;
	printk(KERN_INFO "user wrote: \"%s\" to buffer", buffer);
	if (ret == sizeof(LOGIN) - 1 && memcmp(buffer, from, ret) == 0)
		return (ret);
	else
		return (-EINVAL);
}

static struct file_operations fops = {
	.read = &fortytwo_dev_read,
	.write = &fortytwo_dev_write,
};

static struct miscdevice fortytwo_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = DEVICE_NAME,
	.fops = &fops,
	.mode = 0666,
};

static int __init init(void)
{
	int ret;
	printk(KERN_INFO "Creating fortytwo misc character device.\n");
	ret = misc_register(&fortytwo_dev);
	if (ret)
		return (ret);
	return (0);
}
module_init(init)

static void __exit cleanup(void)
{
	printk(KERN_INFO "Cleaning up module.\n");
	misc_deregister(&fortytwo_dev);
}
module_exit(cleanup)
