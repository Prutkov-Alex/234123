#include <linux/sched.h>

asmlinkage void update_syscall_counter()
{
    current->syscall_counter++;
    return;
}
