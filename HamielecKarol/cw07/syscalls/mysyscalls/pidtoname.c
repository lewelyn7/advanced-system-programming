#include <linux/linkage.h>
#include <linux/kernel.h>
#include <linux/hashtable.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>

#include <linux/spinlock.h>
#include <linux/types.h>

#include <linux/types.h>
#include <linux/sched.h>
#include <linux/slab.h>

asmlinkage long sys_pidtoname(pid_t pid_num, char * res_str) {
    struct task_struct* taskstr = get_pid_task(find_get_pid((int) pid_num), PIDTYPE_PID);
    printk(KERN_WARNING "sys_pidtoname\n");

    if(taskstr == NULL){
        printk(KERN_WARNING "cant find proccess with that PID\n");
        return -EFAULT;
    }
    printk("proccess len: %d", strlen(taskstr->comm));
    if(copy_to_user(res_str, taskstr->comm, strlen(taskstr->comm))){
        printk(KERN_WARNING "copying to user failed\n");
        return -EFAULT;
    }

    return 0;
}
