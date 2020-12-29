#include <linux/module.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/proc_fs.h>
#include <linux/fcntl.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");

#define BUFF_INITIAL_SIZE 40
#define CIRCULAR_MAJOR 199

char * buff;
int buff_size;
int buff_idx = 0;
struct proc_dir_entry * proc_entry = NULL;

/* Operations for /dev/simple */
const struct file_operations circ_fops;

/* Operations for /proc/simple */
const struct file_operations proc_fops;

static int __init circular_init(void)
{
    int result = 0;
    result = register_chrdev(CIRCULAR_MAJOR, "circular", &circ_fops);
	if (result < 0) {
		printk(KERN_WARNING
			"Cannot register the /dev/circular device with major number: %d\n",
			CIRCULAR_MAJOR);
		goto err;
	}

	/* Register an entry in /proc */
	proc_entry = proc_create("circular", 0777, NULL, &proc_fops);
	if (!proc_entry) {
		printk(KERN_WARNING "Cannot create /proc/circular\n");
		goto err;
	}
    buff = kmalloc(BUFF_INITIAL_SIZE, GFP_KERNEL);
    buff_size = BUFF_INITIAL_SIZE;
    if(!buff){
        result = -ENOMEM;
        goto err;
    }else{
        int i = 0;
        for(i = 0; i < BUFF_INITIAL_SIZE; i++){
            buff[i] = '\0';
        }
        result = 0;
        printk(KERN_INFO "The circular mode has been inserted :)\n");
    }
    return result;

err:
	if (proc_entry) {
		proc_remove(proc_entry);
	}
    if(result == -ENOMEM){
        unregister_chrdev(CIRCULAR_MAJOR, "circular");
    }
    kfree(buff); // TODO co jesli sie nie zaalokowalo a my zwalniamy?
    return result;

}

static void __exit circular_exit(void){
    if (proc_entry) {
		proc_remove(proc_entry);
	}
     unregister_chrdev(CIRCULAR_MAJOR, "circular");
     kfree(buff);
     printk(KERN_INFO "The circular module has been removed \n");
}

ssize_t circular_read(struct file *filp, char __user *user_buf, 
    size_t count, loff_t *f_pos)
{
    size_t to_copy = buff_size;
	if (*f_pos >= to_copy) {
		return 0;
	}

	if (copy_to_user(user_buf, buff, to_copy)) {
		printk(KERN_WARNING "CIRCULAR: could not copy data to user\n");
		return -EFAULT;
	}

	*f_pos += to_copy;
	return to_copy;
}

ssize_t circular_write(struct file *filp, const char  *user_buf,
	size_t count, loff_t *f_pos)
{

    size_t tmp_cnt;
    size_t count_inital = count;
    *f_pos = (*f_pos + buff_idx)%buff_size;
    while(count > 0){
        // printk(KERN_WARNING "F_pos %d;", f_pos);
        if(count + *f_pos >= buff_size){
            tmp_cnt = buff_size - *f_pos;
        }else{
            tmp_cnt = count;
        }
        copy_from_user(buff + *f_pos, user_buf + (count_inital-count), tmp_cnt);
        count -= tmp_cnt;
        *f_pos = (*f_pos + tmp_cnt)%buff_size;
        // printk("F_pos %lld; tmp_cnt: %ld\n", *f_pos, tmp_cnt);
    }
    
    buff_idx = *f_pos;

	return count_inital;
}


ssize_t circular_write_proc(struct file *filp, const char   *user_buf,
	size_t count, loff_t *f_pos)
{
    int result = 0;
    long long num;
    char *num_buf;
    num_buf = kmalloc((int)count+1, GFP_KERNEL);
    num_buf[count] = '\0';
	if (copy_from_user(num_buf, user_buf, count)) {
		printk(KERN_WARNING "SIMPLE: could not copy data from user\n");
		return -EFAULT;
	}
    result = kstrtoll(num_buf, 10, (long long *) &num);
    if(result != 0){
        printk(KERN_WARNING "CIRCULAR: conversion to integer error");
    }
    printk("buff size set to: %lld\n", num);
    kfree(num_buf);
    buff = krealloc(buff, num, GFP_KERNEL);
    if(num > buff_size){
        memset(buff+buff_size, '\0', num-buff_size);
    }
    buff_size = num;
    if(buff_idx >= buff_size){
        buff_idx = 0;
    }
    return count;


}
ssize_t circular_read_proc(struct file *filp, char __user *user_buf,
	size_t count, loff_t *f_pos)
{
    return 0;
}

const struct file_operations proc_fops = {
	.read = circular_read_proc,
	.write = circular_write_proc,
};
const struct file_operations circ_fops = {
	.read = circular_read,
    .write = circular_write
};
module_init(circular_init);
module_exit(circular_exit);