// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/uaccess.h>
#include <linux/errno.h>
#include <linux/dcache.h>
#include <linux/debugfs.h>
#include <linux/syscalls.h>
#include <linux/jiffies.h>
#include <linux/mutex.h>

MODULE_ALIAS("fortytwo_debugfs");
MODULE_LICENSE("GPL v2");

#define LOG "fortytwo_dir: "
#define DEVICE_NAME "fortytwo"
#define LOGIN "sclolus"

static ssize_t id_read(struct file *file, char __user *to, size_t size, loff_t *_offset)
{
	int64_t ret;

	ret = simple_read_from_buffer(to, size, _offset, LOGIN, sizeof(LOGIN) - 1);
	if (ret < 0)
		return (-EIO);
	return (ret);
}

static ssize_t id_write(struct file *file, const char __user *from, size_t size, loff_t *_offset)
{
	static char buffer[sizeof(LOGIN)];
	uint64_t    ret;

	if (size != sizeof(LOGIN) - 1)
		return (-EINVAL);
	ret = copy_from_user(buffer, from, sizeof(LOGIN) - 1);
	if (ret == 0 && memcmp(buffer, LOGIN, sizeof(LOGIN) - 1) == 0)
		return (sizeof(LOGIN) - 1);
	else
		return (-EINVAL);
}

static struct mutex foo_mutex;
static char	    foo_buffer[PAGE_SIZE];

static ssize_t	foo_read(struct file *file, char __user *to, size_t size, loff_t *_offset)
{
	int64_t    ret;

	mutex_lock(&foo_mutex);
	ret = simple_read_from_buffer(to, size, _offset, foo_buffer, sizeof(foo_buffer));
	mutex_unlock(&foo_mutex);
	if (ret < 0)
		return (-EIO);
	return (ret);
}

static ssize_t	foo_write(struct file *file, const char __user *from, size_t size, loff_t *_offset)
{
	int64_t    ret;

	mutex_lock(&foo_mutex);
	ret = simple_write_to_buffer(foo_buffer, sizeof(foo_buffer), _offset, from, size);
	mutex_unlock(&foo_mutex);
	if (ret < 0)
		return (-EIO);
	return (ret);
}

static struct file_operations id_fops = {
	.read = &id_read,
	.write = &id_write,
};

static struct file_operations foo_fops = {
	.read = &foo_read,
	.write = &foo_write,
};

static struct dentry *fortytwo_dir = NULL;
static struct dentry *id_file = NULL;
static struct dentry *jiffies_file = NULL;
static struct dentry *foo_file = NULL;

static int __init init(void)
{
	mutex_init(&foo_mutex);
	fortytwo_dir = debugfs_create_dir("fortytwo", NULL);
	printk(KERN_INFO LOG "Creating fortytwo debugfs hierarchy");
	if (fortytwo_dir == NULL) {
		printk(KERN_WARNING LOG "Failed to create fortytwo subdirectory");
		return (-EPERM);
	}
	id_file = debugfs_create_file("id", 0666, fortytwo_dir, NULL, &id_fops);
	if (id_file == NULL) {
		printk(KERN_WARNING LOG "Failed to create id file");
		goto err_cleanup;
	}
	jiffies_file = debugfs_create_u64("jiffies", 0444, fortytwo_dir, (u64 *)&jiffies); // change the fops
	if (jiffies_file == NULL) {
		printk(KERN_WARNING LOG "Failed to create jiffies file");
		goto err_cleanup;
	}
	foo_file = debugfs_create_file("foo", 0644, fortytwo_dir, NULL, &foo_fops); // change the fops
	if (foo_file == NULL) {
		printk(KERN_WARNING LOG "Failed to create foo file");
		goto err_cleanup;
	}
	return (0);
err_cleanup:
	debugfs_remove_recursive(fortytwo_dir);
	return (-EPERM);
}
module_init(init)

	static void __exit cleanup(void)
{
	printk(KERN_INFO LOG "Cleaning up module.\n");
	debugfs_remove_recursive(fortytwo_dir);
}
module_exit(cleanup)
