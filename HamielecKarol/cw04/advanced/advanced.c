// SPDX-License-Identifier: Unlicense

#include <linux/fs.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/uaccess.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/slab.h>
#include <linux/namei.h>
#include <linux/seq_file.h>
#include <linux/mount.h>
#include <linux/sched/task.h>

/* prname device code */

static char process_name[TASK_COMM_LEN];
static char process_name_len = -1;
static const char prname_devname[] = "prname";

static ssize_t advanced_prname_read(struct file *, char *, size_t, loff_t *);
static ssize_t advanced_prname_write(struct file *, const char *,
				     size_t, loff_t *);

static const struct file_operations prname_file_ops = {
	.owner = THIS_MODULE,
	.read = advanced_prname_read,
	.write = advanced_prname_write
};

static struct miscdevice prname_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = prname_devname,
	.fops = &prname_file_ops,
	.mode = 00666
};

static ssize_t advanced_prname_read(struct file *file, char __user *buf,
				    size_t len, loff_t *pos)
{
	size_t bytes_to_copy;
	unsigned long bytes_not_copied, bytes_copied;

	if (process_name_len < 0)
		return -EBUSY;

	if (*pos < 0)
		return -EINVAL;

	if (!len || *pos >= process_name_len)
		return 0;

	bytes_to_copy = min((size_t) (process_name_len - *pos), len);

	bytes_not_copied =
		copy_to_user(buf, process_name + *pos, bytes_to_copy);

	if (bytes_not_copied)
		return -EFAULT;

	*pos += bytes_copied = bytes_to_copy - bytes_not_copied;

	return bytes_copied;
}

static ssize_t advanced_prname_write(struct file *file, const char __user *buf,
				     size_t len, loff_t *pos)
{
	int rc;
	/* pid_t is almost always an int, but for safety we'll use long */
	long _new_pid;
	pid_t new_pid;
	struct pid *new_pidp;
	struct task_struct *taskp;

	rc = kstrtol_from_user(buf, len, 10, &_new_pid);
	if (rc)
		return rc;

	if (_new_pid < 1)
		return -EINVAL;

	new_pid = _new_pid;
	/* check for pid_t overflow */
	if (new_pid != _new_pid)
		return -EINVAL;

	new_pidp = find_get_pid(new_pid);
	if (!new_pidp)
		return -ESRCH;

	taskp = get_pid_task(new_pidp, PIDTYPE_PID);

	put_pid(new_pidp);

	if (!taskp)
		return -ESRCH;

	get_task_comm(process_name, taskp);

	put_task_struct(taskp);

	process_name_len = strlen(process_name);

	return len;
}

/* jiffies device code */

static const char jiffies_devname[] = "jiffies";

static ssize_t advanced_jiffies_read(struct file *, char *, size_t, loff_t *);

static const struct file_operations jiffies_file_ops = {
	.owner = THIS_MODULE,
	.read = advanced_jiffies_read
};

static struct miscdevice jiffies_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = jiffies_devname,
	.fops = &jiffies_file_ops
};

static ssize_t advanced_jiffies_read(struct file *file, char __user *buf,
				     size_t len, loff_t *pos)
{
	size_t bytes_to_copy;
	unsigned long bytes_not_copied, bytes_copied;

	char jiffies_string[30];
	unsigned char jiffies_string_len;

	if (*pos < 0)
		return -EINVAL;

	/* when You use volatile as an atomic... */
	sprintf(jiffies_string, "%lu", jiffies);
	jiffies_string_len = strlen(jiffies_string);

	if (*pos >= jiffies_string_len)
		return 0;

	bytes_to_copy = min((size_t) (jiffies_string_len - *pos), len);

	bytes_not_copied =
		copy_to_user(buf, jiffies_string + *pos, bytes_to_copy);

	if (bytes_not_copied)
		return -EFAULT;

	*pos += bytes_copied = bytes_to_copy - bytes_not_copied;

	return bytes_copied;
}

/* mountderef device code */

static size_t pathname_len;
static char *pathname;
static char *pathname_buf;
static const char mountderef_devname[] = "mountderef";

static ssize_t
advanced_mountderef_write(struct file *file, const char __user *buf,
			  size_t len, loff_t *pos);
static ssize_t advanced_mountderef_read(struct file *file, char __user *buf,
					size_t len, loff_t *pos);

static const struct file_operations mountderef_file_fops = {
	.owner = THIS_MODULE,
	.read = advanced_mountderef_read,
	.write = advanced_mountderef_write,
};

static struct miscdevice mountderef_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = mountderef_devname,
	.fops = &mountderef_file_fops,
	.mode = 00666
};

static ssize_t advanced_mountderef_read(struct file *file, char __user *buf,
					size_t len, loff_t *pos)
{
	size_t bytes_to_copy;
	unsigned long bytes_not_copied, bytes_copied;

	if (!pathname)
		return -EBUSY;

	if (*pos < 0)
		return -EINVAL;

	if (!len || *pos >= pathname_len)
		return 0;

	bytes_to_copy = min((size_t) (pathname_len - *pos), len);

	bytes_not_copied = copy_to_user(buf, pathname + *pos, bytes_to_copy);

	if (bytes_not_copied)
		return -EFAULT;

	*pos += bytes_copied = bytes_to_copy - bytes_not_copied;

	return bytes_copied;
}

static ssize_t
advanced_mountderef_write(struct file *file, const char __user *buf,
			  size_t len, loff_t *pos)
{
	struct path path, root_path;
	ssize_t rc;
	char *new_pathname;
	char *new_pathname_buf = kmalloc(len + 1, GFP_KERNEL);

	if (!new_pathname_buf)
		return -ENOMEM;

	if (copy_from_user(new_pathname_buf, buf, len)) {
		rc = -EFAULT;
		goto out_free_new_pathname;
	}

	new_pathname_buf[len] = 0;

	rc = kern_path(new_pathname_buf, LOOKUP_FOLLOW, &path);
	if (rc)
		goto out_free_new_pathname;

	dput(path.dentry);

	root_path = (struct path) {
		.dentry = dget(path.mnt->mnt_root),
		.mnt = path.mnt
	};

	new_pathname = d_path(&root_path, new_pathname_buf, len + 1);

	path_put(&root_path); /* same as dput() with mntput() */

	if (IS_ERR(new_pathname)) {
		rc = PTR_ERR(new_pathname);
		goto out_free_new_pathname;
	}

	kfree(pathname_buf);

	pathname_buf = new_pathname_buf;
	pathname = new_pathname;
	pathname_len = strlen(new_pathname);

	return len;

out_free_new_pathname:
	kfree(new_pathname_buf);

	return rc;
}

/* module init/exit code */

static int __init advanced_init(void)
{
	int rc;

	rc = misc_register(&prname_dev);
	if (rc)
		goto out;

	rc = misc_register(&jiffies_dev);
	if (rc)
		goto out_deregister_prname;

	rc = misc_register(&mountderef_dev);
	if (rc)
		goto out_deregister_jiffies;

	pr_debug("advanced: module loaded\n");
	return 0;

out_deregister_jiffies:
	misc_deregister(&jiffies_dev);
out_deregister_prname:
	misc_deregister(&prname_dev);
out:
	return rc;
}

static void __exit advanced_exit(void)
{
	misc_deregister(&mountderef_dev);
	if (pathname)
		kfree(pathname_buf);
	misc_deregister(&jiffies_dev);
	misc_deregister(&prname_dev);
	pr_debug("Goodbye, Advanced!\n");
}

module_init(advanced_init);
module_exit(advanced_exit);

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Wojtek Kosior");
MODULE_DESCRIPTION("A module that creates simple character devices 'prname', 'jiffies' and 'mountderef' in /dev");
