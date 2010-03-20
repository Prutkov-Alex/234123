#include <linux/sched.h>

asmlinkage void update_syscall_counter()
{
    current->syscall_counter++;
    return;
}

asmlinkage int sys_get_num_syscalls(int pid)
{
    if(pid == 0)
    {
	return current->syscall_counter;
    }
    if(pid == 1)
    {
	return current->p_pptr->syscall_counter;
    }
    return (find_task_by_pid(pid))->syscall_counter;
}

asmlinkage int sys_get_max_proc_syscalls(int *syscall_num)
{
    /* TODO: We have first to check that syscall_num is a valid pointer */
    long max_syscall_num  = current->syscall_counter;
    pid_t max_syscall_pid = current->pid;
    task_t *task = current->next_task;
    while(task != current)
    {
	if(task->syscall_counter > max_syscall_num || (task->syscall_counter == max_syscall_num && task->pid < max_syscall_pid))
	{
	    max_syscall_num = task->syscall_counter;
	    max_syscall_pid = task->pid;
	}
    }
    *syscall_num = max_syscall_num;
    return max_syscall_pid;
}

asmlinkage int sys_init_syscalls_counters(int pid, int init_value)
{
    task_t *task = NULL;
    if(pid < 0 || init_value < 0)
    {
	return -EINVAL;
    }
    if(pid == 0) 
    {
	task = current;
    }
    else if(pid == 1)
    {
	task = current->p_pptr;
    }
    else
    {
	task = find_task_by_pid(pid);
    }
    if(task == NULL)
    {
	return -ESRCH;
    }
    task->syscall_counter = init_value;
    return 0;
}
