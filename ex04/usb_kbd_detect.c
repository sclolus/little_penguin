// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/kernel.h>
/* #include <linux/delay.h> */
/* #include <linux/kobject.h> */
#include <linux/device.h>
#include <linux/usb.h>
#include <linux/mod_devicetable.h>

MODULE_LICENSE("GPL v2");

# define USB_DEVICE_ID_MATCH_CLASS (USB_DEVICE_ID_MATCH_DEV_CLASS |	\
					USB_DEVICE_ID_MATCH_DEV_SUBCLASS)
# define USB_DEVICE_CLASS(cl, sc) \
	.match_flags = USB_DEVICE_ID_MATCH_CLASS,	\
		.bDeviceClass = (cl),			\
		.bDeviceSubClass = (sc),

static const struct usb_device_id usb_module_id_table[2] = {
	{ USB_DEVICE_CLASS(0x09, 0x00) },
	{}
};
MODULE_DEVICE_TABLE(usb, usb_module_id_table);

static int __init init(void)
{
	printk(KERN_INFO "An usb keyboard has been plugged.\n");
	return (0);
}
module_init(init)

static void __exit cleanup(void)
{
	printk(KERN_INFO "Cleaning up module.\n");
}
module_exit(cleanup)
