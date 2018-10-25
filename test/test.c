#include <stdio.h> // printf
#include <stdlib.h> // wait, sleep
#include <unistd.h> // fork
#include <errno.h> // error codes
#include <assert.h> // assert
#include <sys/resource.h> // setpriority
#include <signal.h> // SIGKILL

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

    /* Get effective user id (0 = roots) */
    uid_t euid = geteuid();

    printf("Error Check: set_myFlag system call\n");

    if (euid != 0) { // Not root
        result_set_myCall = syscall(NR_SET_MYFLAG, pid, MY_FLAG_0);
        assert(result_set_myCall == EACCES);
        printf("\tPASS (without root privileges)\n");
        return 0;
    }

    /* Invalid flag case */
    result_set_myCall = syscall(NR_SET_MYFLAG, pid, MY_FLAG_INVAL);
    assert(result_set_myCall == EINVAL);

    /* Invalid pid case */
    result_set_myCall = syscall(NR_SET_MYFLAG, -1, MY_FLAG_0);
    assert(result_set_myCall == ESRCH);

    printf("\tPASS\n");

    printf("Functionality check: set_myFlag()\n");  

    /* Set flag of the process to 1 */
    result_set_myCall = syscall(NR_SET_MYFLAG, pid, MY_FLAG_1);
    assert(result_set_myCall == SUCCESS);

    printf("\tPASS\n");

    printf("Error Check: myFlag = 1 -> do_fork()\n"); 

    /* Set nice value > 10, flag = 1 -> No child process will be created */
    setpriority(PRIO_PROCESS, pid, PRIO_ACTION);
	
    pid_t child = fork();
    assert(child < 0);

    printf("\tPASS\n");

    printf("Functionality check: do_fork()\n");
		
    /* Set nice value = 0, flag = 1 -> Child process will be created */
    setpriority(PRIO_PROCESS, pid, PRIO_NORMAL);
    pid_t children[5];

    /* Create 5 children */
    int counter;
    for (counter = 0; counter < 5; counter++) {
        children[counter] = fork();
        assert(children[counter] >= 0);

        /* Child process should break the loop */
        if (children[counter] == 0)
            break;
    }

    /* Children should go to sleep for a long time */
    if (children[counter] == 0) {
        sleep(10000);
    } else { // Parent
        /* Create killer child process */
        pid_t killer = fork();
        assert(killer >= 0);

        if(killer > 0) { // Parent
            printf("\tPASS\n");
            printf("Functionality check: do_exit()\n");
        } else { // Killer
            /* Set nice value > 10, flag = 1 -> siblings will be killed */
            setpriority(PRIO_PROCESS, getpid(), PRIO_ACTION);
            exit(EXIT_SUCCESS);
        }
    }

    pid_t waiting;

    /* Parent process waits for all children to be killed */
    while(1) {
        waiting = wait(NULL);

        for (counter = 0; counter < 5; counter++) {
            if (waiting == children[counter]) // Match with child
                break;
        }

        /* Children are killed */
        if (waiting == -1)
            break;
    }

    printf("\tPASS\n");

	return 0;
}
