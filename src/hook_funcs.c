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

    exec_res = orig_execve(p_regs);
    /*
     * #define current get_current()
     * static inline struct task_struct * get_current(void) - return context of current task.
     */
    printk(KERN_INFO "execve '%s' process\n", current->comm);
    return exec_res;
}

