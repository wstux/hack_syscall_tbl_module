#include <linux/module.h>
#include <linux/kernel.h>

static int __init init_hack_syscall_tbl_module(void)
{
    printk(KERN_INFO "init_module\n");   
    return 0;
}
   
static void __exit cleanup_hack_syscall_tbl_module(void)
{
    printk(KERN_INFO "cleanup_module\n");
}
   
module_init(init_hack_syscall_tbl_module);
module_exit(cleanup_hack_syscall_tbl_module);
   
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Chistyakov Alexander");
MODULE_DESCRIPTION("Some description...");

