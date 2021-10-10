#include <linux/syscalls.h>
#include <linux/version.h>

#if LINUX_VERSION_CODE <= KERNEL_VERSION(4, 16, 0)
    #error "Usupported kernel version"
#endif

#include "syscall_utils.h"

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,7,0)
    #define USE_KPROBE 1
#endif

#if USE_KPROBE
    #include <linux/kprobes.h>

    static struct kprobe kp_kallsyms_lookup = { .symbol_name = "kallsyms_lookup_name" };
#endif

static sys_call_table_t* _p_sys_call_table = NULL;
static sys_call_fn_t orig_syscall_table[NR_syscalls];

static unsigned long cr0;

static inline void write_cr0_forced(unsigned long val)
{
    unsigned long force_order;

    asm volatile(
        "mov %0, %%cr0"
        : "+r"(val), "+m"(force_order));
}

#define PROTECT_MEMORY() write_cr0_forced(cr0)
#define UNPROTECT_MEMORY() write_cr0_forced(cr0 & ~0x00010000)

sys_call_table_t* get_syscall_table(void)
{
#ifdef USE_KPROBE
    typedef unsigned long (*kallsyms_lookup_name_t)(const char* name);

    kallsyms_lookup_name_t kallsyms_lookup_name;
#endif

    sys_call_table_t* p_syscall_table = NULL;

    if (_p_sys_call_table != NULL) {
        return _p_sys_call_table;
    }

#ifdef USE_KPROBE
    register_kprobe(&kp_kallsyms_lookup);
    kallsyms_lookup_name = (kallsyms_lookup_name_t)kp_kallsyms_lookup.addr;
    unregister_kprobe(&kp_kallsyms_lookup);
#endif

    p_syscall_table = (sys_call_table_t*)kallsyms_lookup_name("sys_call_table");
    return p_syscall_table;
}

long init_syscall_table(void)
{
    int i;

    _p_sys_call_table = get_syscall_table();
    if (! _p_sys_call_table) {
        return -1;
    }

    cr0 = read_cr0();

    for (i = 0; i < NR_syscalls; ++i) {
        orig_syscall_table[i] = NULL;
    }

    return 0;
}

sys_call_fn_t hook_syscall(sys_call_fn_t hook_syscall_fn, int syscall_num)
{
    if (! _p_sys_call_table) {
        if (init_syscall_table() != 0) {
            return NULL;
        }
    }

    if (! orig_syscall_table[syscall_num]) {
        orig_syscall_table[syscall_num] = (sys_call_fn_t)_p_sys_call_table[syscall_num];
    }

    UNPROTECT_MEMORY();

    _p_sys_call_table[syscall_num] = (unsigned long)hook_syscall_fn;

    PROTECT_MEMORY();

    return orig_syscall_table[syscall_num];
}

long restore_orig_syscall(int syscall_num)
{
    if (! orig_syscall_table[syscall_num]) {
        return 0;
    }

    UNPROTECT_MEMORY();

    _p_sys_call_table[syscall_num] = (unsigned long)orig_syscall_table[syscall_num];

    PROTECT_MEMORY();

    return 0;
}

sys_call_fn_t orig_syscall(int syscall_num)
{
    if (! _p_sys_call_table) {
        return NULL;
    }

    if (orig_syscall_table[syscall_num]) {
        return orig_syscall_table[syscall_num];
    }

    return (sys_call_fn_t)_p_sys_call_table[syscall_num];
}

