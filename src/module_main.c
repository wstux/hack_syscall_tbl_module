#include <linux/version.h>

#if LINUX_VERSION_CODE <= KERNEL_VERSION(4, 16, 0)
    #error "Usupported kernel version"
#endif

#include <linux/module.h>
#include <linux/kernel.h>

#define MODULE_NAME "hack_syscall_tbl"

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,7,0)
    #define KPROBE_LOOKUP 1
    #include <linux/kprobes.h>

    static struct kprobe kp = { .symbol_name = "kallsyms_lookup_name" };
#endif

unsigned long cr0;

static unsigned long *p_sys_call_table;

typedef asmlinkage long (*orig_execve_t)(const struct pt_regs*);
orig_execve_t orig_execve;

unsigned long* get_syscall_table(void)
{
    unsigned long *syscall_table;

#ifdef KPROBE_LOOKUP
    typedef unsigned long (*kallsyms_lookup_name_t)(const char* name);

    kallsyms_lookup_name_t kallsyms_lookup_name;
    register_kprobe(&kp);
    kallsyms_lookup_name = (kallsyms_lookup_name_t)kp.addr;
    unregister_kprobe(&kp);
#endif
    syscall_table = (unsigned long*)kallsyms_lookup_name("sys_call_table");
    return syscall_table;
}

asmlinkage long hack_execve(const struct pt_regs* p_regs)
{
    int exec_res;
    char __user *filename = (char *)p_regs->di;
    char kfilename[NAME_MAX] = {0};

    if (strncpy_from_user(kfilename, filename, NAME_MAX) > 0) {
        printk(KERN_INFO "executable file is '%s'\n", kfilename);
    }

    /*
     * #define current get_current()
     * static inline struct task_struct * get_current(void) - возвращает контекст текущей задачи.
     */
    printk(KERN_INFO "execve '%s' process before orig_execve\n", current->comm);

    exec_res = orig_execve(p_regs);
    printk(KERN_INFO "orig_execve '%s' process\n", current->comm);
    return exec_res;
}

static inline void write_cr0_forced(unsigned long val)
{
    unsigned long force_order;

    asm volatile(
        "mov %0, %%cr0"
        : "+r"(val), "+m"(force_order));
}

#define PROTECT_MEMORY() write_cr0_forced(cr0)
#define UNPROTECT_MEMORY() write_cr0_forced(cr0 & ~0x00010000)

static int __init init_hack_syscall_tbl_module(void)
{
    printk(KERN_INFO "init_module\n");

    p_sys_call_table = get_syscall_table();
    if (!p_sys_call_table) {
        return -1;
    }

    cr0 = read_cr0();
    orig_execve = (orig_execve_t)p_sys_call_table[__NR_execve];

    UNPROTECT_MEMORY();

    p_sys_call_table[__NR_execve] = (unsigned long)hack_execve;

    PROTECT_MEMORY();

    return 0;
}
   
static void __exit cleanup_hack_syscall_tbl_module(void)
{
    printk(KERN_INFO "cleanup_module\n");

    UNPROTECT_MEMORY();

    p_sys_call_table[__NR_execve] = (unsigned long)orig_execve;

    PROTECT_MEMORY();
}
   
module_init(init_hack_syscall_tbl_module);
module_exit(cleanup_hack_syscall_tbl_module);
   
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Chistyakov Alexander");
MODULE_DESCRIPTION("Some description...");

