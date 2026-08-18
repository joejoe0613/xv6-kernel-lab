#include "kernel/types.h"
#include "user/user.h"

void
busy_work(void)
{
    volatile int i;
    for(i = 0; i < 15000000; ++i)
    ;
}

int
main(int argc, char *argv[])
{
    int high_pid, low_pid;

    high_pid = fork();

    if(high_pid < 0){
        printf("fork failed\n");
        exit(1);
    }

    if(high_pid == 0){
        setpriority(getpid(), 5);

        for(int i = 0; i < 20; ++i){
            printf("HIGH priority process running %d\n", i);
            busy_work();
        }

        exit(0);
    }

    low_pid = fork();

    if(low_pid < 0){
        printf("fork failed\n");
        exit(1);
    }

    if(low_pid == 0){
        setpriority(getpid(), 10);

        for(int i = 0; i < 5; ++i){
            printf("LOW priority process running %d\n", i);
            busy_work();
        }

        exit(0);
    }

    wait(0);
    wait(0);

    exit(0);
}