#include <linux/module.h>
#include <linux/kernel.h>

static int __init init(void)
{
	printk(KERN_INFO "Hello world !\n");
	return (0);
}
module_init(init)

static void __exit cleanup(void)
{
	printk(KERN_INFO "Cleaning up module.\n");
}
module_exit(cleanup)
