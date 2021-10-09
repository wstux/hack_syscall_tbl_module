#ifndef SYSCALL_UTILS_H
#define SYSCALL_UTILS_H

struct pt_regs;

typedef unsigned long sys_call_table_t;
typedef asmlinkage long (*sys_call_fn_t)(const struct pt_regs*);

sys_call_table_t* get_syscall_table(void);

#endif // SYSCALL_UTILS_H

