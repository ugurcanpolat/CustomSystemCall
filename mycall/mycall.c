#include <linux/types.h>
#include <linux/syscalls.h>
#include <linux/kernel.h>

asmlinkage long set_myFlag(pid_t pid, int flag) {
	return 0;
}
