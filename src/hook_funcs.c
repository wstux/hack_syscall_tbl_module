#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/syscalls.h>

#include "hook_funcs.h"

sys_call_fn_t orig_execve;

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

