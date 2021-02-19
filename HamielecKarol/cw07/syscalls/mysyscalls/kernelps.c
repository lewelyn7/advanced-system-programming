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

asmlinkage long sys_kernelps(size_t * arr_size, char ** arr){

    struct task_struct * taskstr;
    size_t buffer_size;
    int it = 0;
    buffer_size = *arr_size;

    printk(KERN_WARNING "SYS_KERNELPS\n");
    for_each_process(taskstr){
        if(arr != NULL){
            if(it == buffer_size || arr[it] == NULL){
                //ERROR
                printk(KERN_WARNING "BUFFER TO SMALL");
                return -EFAULT;
            }

        }
        if(copy_to_user(arr[it], taskstr->comm, strlen(taskstr->comm))){
            printk(KERN_WARNING "copying to user failed\n");
            return -EFAULT;
        }        

        it++;
    }
    *arr_size = it+1;
    return 0;

}
