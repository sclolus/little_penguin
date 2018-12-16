// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/usb.h>
#include <linux/hid.h>

MODULE_ALIAS("usb_kbd");
MODULE_LICENSE("GPL v2");

static const struct usb_device_id usb_module_id_table[2] = {
	{ USB_INTERFACE_INFO(
			USB_INTERFACE_CLASS_HID,
			USB_INTERFACE_SUBCLASS_BOOT,
			USB_INTERFACE_PROTOCOL_KEYBOARD) },
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
