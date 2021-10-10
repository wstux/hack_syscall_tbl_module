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

#ifndef SYSCALL_UTILS_H
#define SYSCALL_UTILS_H

struct pt_regs;

/* Type of system call table pointer. */
typedef unsigned long sys_call_table_t;
/* Type of system call functions. */
typedef asmlinkage long (*sys_call_fn_t)(const struct pt_regs*);

/*
 * @brief   Requesting a pointer to the system call table.
 * @return  Pointer to the system call table.
 */
sys_call_table_t* get_syscall_table(void);

/*
 * @brief   System call table initialization.
 * @return  On success, 0 is returned. On failure, -1 is returned.
 */
long init_syscall_table(void);

/*
 * @brief   Replacement system call a custom function.
 * @param   hook_syscall_fn - custom function.
 * @param   syscall_num - system call number.
 * @return  On success, original system call is returned. On failure, NULL is returned.
 */
sys_call_fn_t hook_syscall(sys_call_fn_t hook_syscall_fn, int syscall_num);

/*
 * @brief   Restore the original system call.
 * @param   syscall_num - system call number.
 * @return  On success, 0 is returned. On failure, -1 is returned.
 */
long restore_orig_syscall(int syscall_num);

#endif // SYSCALL_UTILS_H

