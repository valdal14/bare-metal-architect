#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/wait.h>
#include <pthread.h>

/**
 * fork - create a new process
 * The new process (child process) is an exact copy of the calling process except:
 * - ID
 * - Parent ID
 * - Its own Parent descriptors copy  
 * - resource utilizations are set to 0; see setrlimit(2)
 *
 * wait - wait for process termination 
 * A call to wait blocks the calling process until one of its child processes exits or a signal is received.
 *
 * Macros
 * WIFEXITED(status): True if the terminated normally by a call to _exit(2) or exit(3)
 * WIFSIGNALED(status): True if the process terminated due to receipt of a signal.
 * WIFSTOPPED(status): True if the process has not terminated, but has stopped and can be restarted.
 * - This macro can be true only if the wait call specified the WUNTRACED option 
 *   or if the child process is being traced (see ptrace(2)).
 */

int shared_data = 42;

void process(void)
{
    printf("Child Process:  pid_t = %d\n", getpid());
    printf("Changing the global variable from the child process %d\n", getpid());
    shared_data = 100;
    printf("Child Process: shared_data value = %d\n", shared_data); 
    printf("-----------------------------------------------------------\n");
    sleep(1);
}

void *thread(void *arg)
{
    pid_t *id = (pid_t *)arg;
    printf("Thread: pid_t = %d\n", *id);
    printf("Changing the global variable from the thread %d\n", *id);
    printf("-----------------------------------------------------------\n");
    shared_data = 999;
    sleep(1);
    return (void *)&shared_data;
}

int main(void)
{
    pthread_t t;
    pid_t id = getpid();
    
    if(fork() == 0)
    {
        process();
        exit(0);
    }
    else
    {
        wait(NULL);
        printf("Main Process: pid_t = %d\n", id);
        printf("Main Process: shared_data value = %d\n", shared_data); 
        printf("-----------------------------------------------------------\n");
    }

    int *return_value = NULL;

    pthread_create(&t, NULL, thread, (void *)&id);
    pthread_join(t, (void **)return_value);
    
    printf("Thread: shared_data value = %d\n", shared_data); 
    sleep(1);
    printf("Main Process: shared_data value = %d\n", shared_data);

    return 0;
}
