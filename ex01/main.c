// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/kernel.h>

int init_module(void)
{
	printk(KERN_INFO "Hello world !\n");
	return (0);
}
/* module_init(init) */

void cleanup_module(void)
{
	printk(KERN_INFO "Cleaning up module.\n");
}
/* module_exit(cleanup) */
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("sclolus");
