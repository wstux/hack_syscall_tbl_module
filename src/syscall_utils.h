#ifndef SYSCALL_UTILS_H
#define SYSCALL_UTILS_H

typedef unsigned long sys_call_table_t;

sys_call_table_t* get_syscall_table(void);

#endif // SYSCALL_UTILS_H

