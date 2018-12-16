// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/uaccess.h>
#include <linux/errno.h>
#include <linux/proc_fs.h>
#include <linux/proc_ns.h>
#include <linux/fs.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
#include <linux/mnt_namespace.h>
#include <linux/mount.h>
#include <linux/sched.h>
#include <linux/sched/task.h>
#include <linux/path.h>
#include <linux/dcache.h>
#include <linux/namei.h>
#include <linux/kallsyms.h>
#include <linux/nsproxy.h>
#include <../fs/mount.h>
#include <linux/fs_struct.h>

MODULE_ALIAS("metryingnottokilleverything");
MODULE_LICENSE("GPL v2");

#define MODULE_NAME "mymounts"
#define FILE_NAME MODULE_NAME
#define LOG MODULE_NAME ": "

static void *ct_seq_start(struct seq_file *s, loff_t *pos)
{
	loff_t	*spos;

	if (*pos >= 1)
		return NULL;
	spos = kmalloc(sizeof(loff_t), GFP_KERNEL); //yeah this is useless
	if (!spos)
		return NULL;
	*spos = *pos;
	return pos;
}

static void *ct_seq_next(struct seq_file *s, void *v, loff_t *pos)
{
	return NULL;
}

static void ct_seq_stop(struct seq_file *s, void *v)
{
	kfree(v);
}

static struct vfsmount	*(*dl_collect_mounts)(const struct path *path);
static int		(*dl_iterate_mounts)(int (*)(struct vfsmount *, void *),
			void *,
			struct vfsmount *);

static int		ct_show_mount(struct vfsmount *mount, void *data)
{
	/* struct path	    *root_path = ((struct seq_file*)data)->private; */
	struct super_block  *sb;
	struct path	    dev_path = { .mnt = mount,
					 .dentry = mount->mnt_root };

	sb = mount->mnt_sb;
	if (sb->s_op->show_devname) {
		sb->s_op->show_devname(data, mount->mnt_root);
	} else {
		seq_printf(data, "%s", sb->s_id);
	}
	seq_puts(data, "\t\t");
	seq_path(data, (const struct path *)&dev_path, "\t\n\b\r\a ");
	seq_putc(data, '\n');
	return (0);
}

static int ct_seq_show(struct seq_file *s, void *v)
{
	struct vfsmount	   *mounts;

	dl_collect_mounts = (void *)kallsyms_lookup_name("collect_mounts");
	dl_iterate_mounts = (void *)kallsyms_lookup_name("iterate_mounts");
	if (dl_collect_mounts == NULL || dl_iterate_mounts == NULL)
		return -EPERM;
	mounts = dl_collect_mounts((const struct path *)&current->fs->root);
	if (IS_ERR(mounts))
		return -EPERM;
	return (dl_iterate_mounts(&ct_show_mount, s, mounts));
}

static const struct seq_operations  ct_seq_ops = {
	.start = ct_seq_start,
	.next = ct_seq_next,
	.stop = ct_seq_stop,
	.show = ct_seq_show,
};

static int  mymounts_open(struct inode *inode, struct file *file)
{
	struct path	   *root;

	root = __seq_open_private(file, &ct_seq_ops, sizeof(struct path));
	if (IS_ERR(root))
		return -ENOMEM;
	*root = current->fs->root;
	return 0;
}

static const struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = &mymounts_open,
	.release = &seq_release_private,
	.read = &seq_read,
	.llseek = seq_lseek,
};


static struct proc_dir_entry *mounts_file = NULL;

static int __init init(void)
{
	mounts_file = proc_create(FILE_NAME, 0444, NULL, &fops);
	if (mounts_file == NULL) {
		printk(KERN_WARNING LOG "Failed to create " FILE_NAME " /proc file");
		return -EPERM;
	}
	return (0);
}
module_init(init);

static void __exit cleanup(void)
{
	printk(KERN_INFO LOG "Cleaning up module.\n");
	proc_remove(mounts_file);
}
module_exit(cleanup);
