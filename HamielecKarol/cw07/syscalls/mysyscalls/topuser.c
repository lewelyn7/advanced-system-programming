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

struct uid_data {
     uid_t uid;
     int data;
     struct hlist_node hash_list;
};       

asmlinkage long sys_topuser(uid_t * uid_ret) {
    struct task_struct* proc;

    DEFINE_HASHTABLE(proc_user_tab, 8);

    struct uid_data * cursor_data;
    struct uid_data *tmp;
    int max_cnt = 0;
    int bkt = 0;
    uid_t max_uid = 0;
    bool added;
    printk(KERN_WARNING "sys_topuser\n");
    hash_init(proc_user_tab);
    for_each_process(proc){
        added = false;
        hash_for_each_possible(proc_user_tab, cursor_data, hash_list, proc->cred->uid.val){
            if(proc->cred->uid.val == cursor_data->uid){
                cursor_data->data++;
                added = true;
                if(max_cnt < cursor_data->data){
                    max_cnt = cursor_data->data;
                    max_uid = cursor_data->uid;
                }                
            }
        }
        if(!added){
            tmp = kmalloc(sizeof(struct uid_data), GFP_KERNEL);
            tmp->uid = proc->cred->uid.val;
            tmp->data = 1;
            hash_add(proc_user_tab, &(tmp->hash_list), tmp->uid);
            if(max_cnt < tmp->data){
                max_cnt = tmp->data;
                max_uid = tmp->uid;
            }
        }
        
    }
    hash_for_each(proc_user_tab, bkt, cursor_data, hash_list){
        kfree(cursor_data);
    }
    *uid_ret = max_uid;
return 0;
}
