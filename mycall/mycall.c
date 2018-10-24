#include <linux/types.h>
#include <linux/syscalls.h>
#include <linux/kernel.h>

asmlinkage long set_myFlag(pid_t pid, int flag) {
	
	/* Is value of flag valid? */
	if (flag != 0 && flag != 1) 
		return -EINVAL;
	
	/* Root access? */
	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;
	
	struct task_struct *task;
	task = pid_task(find_vpid(pid), PIDTYPE_PID);
	
	/* Unable to get find task */
	if (task == NULL) 
		return -ESRCH;
		
	/* Set flag */
	task->myFlag = flag;
	
	printk("Flag has been set as %d for process with pid: %d\n", flag, pid);
	
	return 0;
}
