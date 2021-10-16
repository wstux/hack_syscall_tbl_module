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

#include "hook_funcs.h"
#include "syscall_utils.h"

#define MODULE_NAME "hack_syscall_tbl"

/*
 * @brief   Module initialization.
 */
static int __init init_hack_syscall_tbl_module(void)
{
    printk(KERN_INFO "init_module\n");

    if (init_syscall_table() != 0) {
        printk(KERN_WARNING "syscall_table not found\n");
        return -1;
    }

    orig_execve = hook_syscall(hack_execve, __NR_execve);
    if (! orig_execve) {
        printk(KERN_ERR "failed hook sys_execve\n");
        return -1;
    }

    return 0;
}

/*
 * @brief   Module cleanup.
 */
static void __exit cleanup_hack_syscall_tbl_module(void)
{
    printk(KERN_INFO "cleanup_module\n");

    restore_orig_syscall(__NR_execve);
}

module_init(init_hack_syscall_tbl_module);
module_exit(cleanup_hack_syscall_tbl_module);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Chistyakov Alexander");
MODULE_DESCRIPTION("Some description...");
MODULE_VERSION("0.0.1");

