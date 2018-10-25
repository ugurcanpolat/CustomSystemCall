#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <sys/resource.h>
#include <signal.h>

#define NR_SET_MYFLAG 355

#define SUCCESS 0

#define MY_FLAG_0      0
#define MY_FLAG_1      1
#define MY_FLAG_INVAL 15

#define PRIO_NORMAL  0
#define PRIO_ACTION 30

int main(void) {

    long result_set_myCall;
    pid_t pid = getpid();

    uid_t euid = geteuid();

    printf("Error Check: set_myFlag system call\n");

    if (euid != 0) {
        result_set_myCall = syscall(NR_SET_MYFLAG, pid, MY_FLAG_0);
        assert(result_set_myCall == EACCES);
        printf("\tPASS (without root privileges)\n");
        return 0;
    }

    result_set_myCall = syscall(NR_SET_MYFLAG, pid, MY_FLAG_INVAL);
    assert(result_set_myCall == EINVAL);

    result_set_myCall = syscall(NR_SET_MYFLAG, -1, MY_FLAG_0);
    assert(result_set_myCall == ESRCH);

    printf("\tPASS\n");

    printf("Functionality check: set_myFlag()\n");  

	result_set_myCall = syscall(NR_SET_MYFLAG, pid, MY_FLAG_1);
    assert(result_set_myCall == SUCCESS);

    printf("\tPASS\n");

    printf("Error Check: myFlag = 1 -> do_fork()\n"); 

	setpriority(PRIO_PROCESS, pid, PRIO_ACTION);
	
	pid_t child = fork();
	assert(child < 0);

    printf("\tPASS\n");

    printf("Functionality check: do_fork()\n");
		
	setpriority(PRIO_PROCESS, pid, PRIO_NORMAL);
    pid_t children[5];

    int counter;
    for (counter = 0; counter < 5; counter++) {
        children[counter] = fork();
        assert(children[counter] >= 0);

        if (children[counter] == 0) {
            break;
        }
    }

    if (children[counter] == 0) {
        sleep(10000);
    } else {
        pid_t killer = fork();
        assert(killer >= 0);

        if(killer > 0) {
            printf("\tPASS\n");
            printf("Functionality check: do_exit()\n");
        } else {
            setpriority(PRIO_PROCESS, getpid(), PRIO_ACTION);
            exit(EXIT_SUCCESS);
        }
    }

    pid_t waiting;

    while(1) {
        waiting = wait(NULL);

        for (counter = 0; counter < 5; counter++) {
            if (waiting == children[counter])
                break;
        }

        if (waiting == -1)
            break;

    }

    printf("\tPASS\n");

	return 0;
}
