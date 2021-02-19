#include <linux/linkage.h>
#include <linux/kernel.h>
#include <linux/hashtable.h>
#include <linux/syscalls.h>

#include <linux/spinlock.h>
#include <linux/types.h>

#include <linux/types.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/namei.h>
#include <linux/mount.h>
#include <linux/fs.h>
#include <linux/statfs.h>

//TODO CLEAR INCLUDES FROM useless includes


asmlinkage long sys_freeblocks(char * path, u64 * free_blocks) {
    struct kstatfs fsstat_str;
    struct path path_str;
    printk(KERN_WARNING "sys_freeblocks\n");
    if(kern_path(path, LOOKUP_FOLLOW, &path_str)){
        printk(KERN_WARNING "cant get path struct\n");
        return -EFAULT;
    }
    if(vfs_statfs(&path_str, &fsstat_str)){
        printk(KERN_WARNING "Unable to obtain kstatfs struct\n");
        return -EFAULT;
    }    
    *free_blocks = fsstat_str.f_bfree;
    return 0;
}
