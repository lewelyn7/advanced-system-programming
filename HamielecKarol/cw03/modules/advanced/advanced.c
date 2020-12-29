#include <linux/module.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/proc_fs.h>
#include <linux/fcntl.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/miscdevice.h>
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

#include <linux/sched/task.h>

MODULE_AUTHOR("Karol Hamielec");
MODULE_DESCRIPTION("PS labs 3 zad2");
MODULE_LICENSE("GPL");


char task_name[TASK_COMM_LEN];
int task_name_len = 0;
char *path_to_find;
int path_to_find_len;

ssize_t prname_write(struct file *filp, const char __user *user_buf,
	size_t count, loff_t *f_pos)
{
    long pid;
    char num_from_user[20];
    struct pid *found_pid;
    struct task_struct *task;
    int i;
    for(i = 0; i < 20; i++){
        num_from_user[i] = '\0';
    }
	if (copy_from_user(num_from_user, user_buf, count)) {
		printk(KERN_WARNING "PRNAME: could not copy data from user\n");
		return -EFAULT;
	}

    if(kstrtol(num_from_user, 10, &pid)){
        printk(KERN_WARNING "PRNAME: conversion to int error");
        return -EFAULT;
    }
    if(pid  < 1){
        printk(KERN_WARNING "PRNAME: number must be bigger than 1");
        return -EINVAL;
    }
    found_pid = find_get_pid(pid);
    task = get_pid_task(found_pid, PIDTYPE_PID);
    put_pid(found_pid);

    if(!task){
        printk(KERN_WARNING "PRNAME: get task error");
        return -EFAULT;
    }

    get_task_comm(task_name, task);
    put_task_struct(task);
    task_name_len = strlen(task_name);
    return(task_name_len);


}
ssize_t prname_read(struct file *filp, char __user *user_buf,
	size_t count, loff_t *f_pos)
{
    if((*f_pos) >= task_name_len){
        return 0;
    }
    
    if (copy_to_user(user_buf, task_name, task_name_len)){
        printk(KERN_WARNING "PRNAME: could not copy data to user\n");
        return -EFAULT;
    }
    copy_to_user(user_buf + task_name_len, "\r\n", 2);
    *f_pos += task_name_len+2;

    return task_name_len+2;
}
struct file_operations prname_fops = {
    .read = prname_read,
    .write = prname_write
};
static struct miscdevice prname_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "prname",
	.fops = &prname_fops,
    .mode = 00666
};


ssize_t jiffies_read(struct file *filp, char __user *user_buf,
	size_t count, loff_t *f_pos)
{

	char jiffies_buff[45];
	int jiffies_buff_len;


	sprintf(jiffies_buff, "%lu\r\n", jiffies);
	jiffies_buff_len = strlen(jiffies_buff);

	if (*f_pos >= jiffies_buff_len) {
		return 0;
	}	
	
	if (copy_to_user(user_buf, jiffies_buff, jiffies_buff_len)) {
		printk(KERN_WARNING "SIMPLE: could not copy data to user\n");
		return -EFAULT;
	}
	*f_pos += jiffies_buff_len;

	return jiffies_buff_len;
}
struct file_operations jiffies_fops = {
    .read = jiffies_read
    
};
static struct miscdevice jiffies_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "jiffies",
	.fops = &jiffies_fops,
    .mode = 00666
};

// char *mounderef_pathname = NULL;

// ssize_t mountderef_read(struct file *filp, char __user *user_buf,
// 	size_t count, loff_t *f_pos)
// {
//     int pathname_len ;
//     if(!mounderef_pathname){
//         return -EINVAL;
//     }
//     pathname_len = strlen(mounderef_pathname);

//     if(*f_pos >= pathname_len){
//         return 0;

//     }

// 	if(copy_to_user(user_buf, mounderef_pathname, pathname_len)) {
// 		printk(KERN_WARNING "MOUNTDEREF: could not copy data to user\n");
// 		return -EFAULT;
// 	}

//     *f_pos += pathname_len;
//     return count;
// }

// ssize_t mountderef_write(struct file *filp, const char __user *user_buf,
// 	size_t count, loff_t *f_pos)
// {
//     struct path path, found_path;
//     int res;
//     char* found_pathname;
//     char *path_to_find = kmalloc(count + 1, GFP_KERNEL);
//     if(!path_to_find){
//         printk(KERN_WARNING "Error with space allocation");
//         res = -ENOMEM;
//         return count;
//     }
// 	if (copy_from_user(path_to_find, user_buf, count)) {
// 		printk(KERN_WARNING "MOUNTDEREF: could not copy data from user\n");		
//         res= -EFAULT;
//         goto err;
// 	}    
//     path_to_find[count] = '\0';

//     printk(KERN_WARNING "%s", path_to_find);
//     res = kern_path(path_to_find, LOOKUP_FOLLOW, &path); //TODO
//     if(res){
//  		printk(KERN_WARNING "MOUNTDEREF: could not find path: %d\n", res);		
//         res =  -EFAULT;
//         goto err;
// 	}         
//     dput(path.dentry);
    

// 	found_path = (struct path) {
// 		.dentry = dget(path.mnt->mnt_root),
// 		.mnt = path.mnt
// 	};
//     found_pathname = d_path(&found_path, path_to_find, count + 1);
//     path_put(&found_path);

//     if(mounderef_pathname) 
//         kfree(mounderef_pathname);
//     mounderef_pathname = found_pathname;


// err:
//     kfree(path_to_find);
//     return count;
// }
// struct file_operations mountderef_fops = {
//     .read = mountderef_read,
//     .write = mountderef_write
    
// };
// static struct miscdevice mountderef_dev = {
// 	.minor = MISC_DYNAMIC_MINOR,
// 	.name = "mounderef",
// 	.fops = &mountderef_fops,
//     .mode = 00666
// };

static int __init zad2_init(void)
{
    misc_register(&prname_dev);
    misc_register(&jiffies_dev);
    // misc_register(&mountderef_dev);

        return 0;
}

static void __exit zad2_exit(void)
{
    misc_deregister(&prname_dev);
    misc_deregister(&jiffies_dev);
    // misc_deregister(&mountderef_dev);
}

module_init(zad2_init);
module_exit(zad2_exit);