#include <linux/sched.h>
#include <asm/uaccess.h>

asmlinkage void update_syscall_counter()
{
    current->syscall_counter++;
    return;
}

asmlinkage int sys_get_num_syscalls(int pid)
{
    task_t *task;
    if(pid < 0)
    {
		return -EINVAL;
    }
    if(pid == 0 || current->pid == pid)
    {
		return current->syscall_counter+1;
    }
    if(pid == 1)
    {
		return current->p_pptr->syscall_counter;
    }
    task = find_task_by_pid(pid);
    if(task) {
		return task->syscall_counter;
    }
    return -ESRCH;
}

asmlinkage int sys_get_max_proc_syscalls(int *syscall_num)
{
    if(!access_ok(VERIFY_WRITE,syscall_num,sizeof(int)))
    {
		return -EFAULT;
    }
    long max_syscall_num  = current->syscall_counter+1;
    pid_t max_syscall_pid = current->pid;
    task_t *task;
    for_each_task(task)
    {
		if(task->syscall_counter > max_syscall_num || (task->syscall_counter == max_syscall_num && task->pid < max_syscall_pid))
		{
			max_syscall_num = task->syscall_counter;
			max_syscall_pid = task->pid;
		}
    }
	if(syscall_num == NULL) return max_syscall_pid;
    if(copy_to_user(syscall_num,&max_syscall_num,sizeof(int))) return -EFAULT;
    return max_syscall_pid;
}

asmlinkage int sys_init_syscalls_counters(int pid, int init_value)
{
    task_t *task = NULL;
    if(pid < 0 || init_value < 0)
    {
		return -EINVAL;
    }
    if(pid == 0 || current->pid == pid) 
    {
		current->syscall_counter = init_value - 1;
		return 0;
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
