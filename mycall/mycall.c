#include <linux/types.h>
#include <linux/syscalls.h>
#include <linux/kernel.h>

asmlinkage long set_myFlag(pid_t pid, int flag) {
	struct task_struct *task;
	
	task = pid_task(find_vpid(pid), PIDTYPE_PID);
	
	task->myFlag = flag;
	
	return 0;
}
