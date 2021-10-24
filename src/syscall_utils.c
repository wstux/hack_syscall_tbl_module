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

#include <linux/syscalls.h>
#include <linux/version.h>

#include "syscall_utils.h"

#if LINUX_VERSION_CODE <= KERNEL_VERSION(4, 16, 0)
    #error "Usupported kernel version"
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,7,0)
    #define USE_KPROBE 1
#endif

#if LINUX_VERSION_CODE > KERNEL_VERSION(4, 4, 0)
    #define USE_KALLSYMS_LOOKUP_FUNC 1
#endif

#if defined(USE_KALLSYMS_LOOKUP_FUNC) && defined(USE_KPROBE)
    #include <linux/kprobes.h>
#endif

#define PAGE_RW 0x00010000

/* System call table pointer. */
static sys_call_table_t* _p_sys_call_table = NULL;
/* Original system call table. */
static sys_call_fn_t _orig_syscall_table[NR_syscalls];

static inline void write_cr0_forced(unsigned long val)
{
    unsigned long force_order;

    asm volatile(
        "mov %0, %%cr0"
        : "+r"(val), "+m"(force_order));
}

#define PROTECT_MEMORY() \
    write_cr0_forced((read_cr0() & ~PAGE_RW) ? read_cr0() | PAGE_RW : read_cr0())
#define UNPROTECT_MEMORY() \
    write_cr0_forced(read_cr0() & ~PAGE_RW)

/*
 * @details To write.
 */
sys_call_table_t* get_syscall_table(void)
{
#ifdef USE_KPROBE
    typedef unsigned long (*kallsyms_lookup_name_t)(const char* name);

    kallsyms_lookup_name_t kallsyms_lookup_name;
    struct kprobe kp_kallsyms_lookup = { .symbol_name = "kallsyms_lookup_name" };
#endif

    sys_call_table_t* p_syscall_table = NULL;
    if (_p_sys_call_table != NULL) {
        return _p_sys_call_table;
    }

#ifdef USE_KALLSYMS_LOOKUP_FUNC
    #ifdef USE_KPROBE
    register_kprobe(&kp_kallsyms_lookup);
    kallsyms_lookup_name = (kallsyms_lookup_name_t)kp_kallsyms_lookup.addr;
    unregister_kprobe(&kp_kallsyms_lookup);
    #endif // USE_KPROBE

    p_syscall_table = (sys_call_table_t*)kallsyms_lookup_name("sys_call_table");
#else // USE_KALLSYMS_LOOKUP_FUNC
    unsigned long int i;

    for (i = (sys_call_table_t)sys_close; i < ULONG_MAX; i += sizeof(void*)) {
        if(((sys_call_table_t*)i)[__NR_close] == (sys_call_table_t)sys_close) {
            p_syscall_table = (sys_call_table_t*)i;
            break;
        }
    }
#endif // USE_KALLSYMS_LOOKUP_FUNC

    return p_syscall_table;
}

/*
 * @details To write.
 */
long init_syscall_table(void)
{
    int i;

    _p_sys_call_table = get_syscall_table();
    if (! _p_sys_call_table) {
        return -1;
    }

    for (i = 0; i < NR_syscalls; ++i) {
        _orig_syscall_table[i] = NULL;
    }

    return 0;
}

/*
 * @details To write.
 */
sys_call_fn_t hook_syscall(sys_call_fn_t hook_syscall_fn, int syscall_num)
{
    if (! _p_sys_call_table) {
        return NULL;
    }
    if (syscall_num >= NR_syscalls) {
        return NULL;
    }

    if (! _orig_syscall_table[syscall_num]) {
        _orig_syscall_table[syscall_num] = (sys_call_fn_t)_p_sys_call_table[syscall_num];
    }

    UNPROTECT_MEMORY();

    _p_sys_call_table[syscall_num] = (unsigned long)hook_syscall_fn;

    PROTECT_MEMORY();

    return _orig_syscall_table[syscall_num];
}

/*
 * @details To write.
 */
sys_call_fn_t orig_syscall(int syscall_num)
{
    if (! _p_sys_call_table) {
        return NULL;
    }
    if (syscall_num >= NR_syscalls) {
        return NULL;
    }

    if (_orig_syscall_table[syscall_num]) {
        return _orig_syscall_table[syscall_num];
    }

    return (sys_call_fn_t)_p_sys_call_table[syscall_num];
}

/*
 * @details To write.
 */
long restore_orig_syscall(int syscall_num)
{
    if (! _p_sys_call_table) {
        return -1;
    }
    if (syscall_num >= NR_syscalls) {
        return -1;
    }
    if (! _orig_syscall_table[syscall_num]) {
        return 0;
    }

    UNPROTECT_MEMORY();

    _p_sys_call_table[syscall_num] = (unsigned long)_orig_syscall_table[syscall_num];

    PROTECT_MEMORY();

    return 0;
}

/*
 * @details To write.
 */
long restore_orig_syscalls(void)
{
    int i;

    for (i = 0; i < NR_syscalls; ++i) {
        if (restore_orig_syscall(i) != 0) {
            return -1;
        }
    }

    return 0;
}

#undef PAGE_RW
#undef PROTECT_MEMORY
#undef UNPROTECT_MEMORY
#if defined(USE_KALLSYMS_LOOKUP_FUNC)
    #undef USE_KALLSYMS_LOOKUP_FUNC
#endif
#if defined(USE_KPROBE)
    #undef USE_KPROBE
#endif

