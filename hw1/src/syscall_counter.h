int get_num_syscalls(int pid)
{
    unsigned int res;
    __asm__
    (
	"movl $243, %%eax;"
	"movl %1, %%ebx;"
	"int $0x80;"
	"movl %%eax,%0"
	: "=m" (res)
	: "m" (pid)
	: "%eax","%ebx","%ecx","%edx"
    );
    if (res>= (unsigned long)(-125))
    {
        errno = -res;
        res = -1;
    }
    return (int) res;
}

int get_max_proc_syscalls(int *syscall_num)
{
    unsigned int res;
    __asm__
    (
	"movl $244, %%eax;"
	"movl %1, %%ebx;"
	"int $0x80;"
	"movl %%eax,%0"
	: "=m" (res)
	: "m" (syscall_num)
	: "%eax","%ebx","%ecx","%edx"
    );
    if (res>= (unsigned long)(-125))
    {
        errno = -res;
        res = -1;
    }
    return (int) res;
}

int init_syscalls_counters(int pid, int init_value)
{
    unsigned int res;
    __asm__
    (
	"movl $245, %%eax;"
	"movl %1, %%ebx;"
	"movl %2, %%ecx;"
	"int $0x80;"
	"movl %%eax,%0"
	: "=m" (res)
	: "m" (pid) ,"m" (init_value)
	: "%eax","%ebx","%ecx","%edx"
    );
    if (res>= (unsigned long)(-125))
    {
        errno = -res;
        res = -1;
    }
    return (int) res;
}
