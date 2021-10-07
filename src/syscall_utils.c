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

sys_call_table_t* get_syscall_table(void)
{
    sys_call_table_t* p_syscall_table;

#ifdef USE_KPROBE
    typedef unsigned long (*kallsyms_lookup_name_t)(const char* name);

    kallsyms_lookup_name_t kallsyms_lookup_name;
    register_kprobe(&kp_kallsyms_lookup);
    kallsyms_lookup_name = (kallsyms_lookup_name_t)kp_kallsyms_lookup.addr;
    unregister_kprobe(&kp_kallsyms_lookup);
#endif

    p_syscall_table = (sys_call_table_t*)kallsyms_lookup_name("sys_call_table");
    return p_syscall_table;
}

