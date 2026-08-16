#include "kernel/types.h"
#include "user/user.h"

void
busy_work(void)
{
    volatile int i;
    for(i = 0; i < 20000000; ++i)
        ;
}

int
main(int argc, char * argv[])
{
    int pid1, pid2;

    pid1 = fork();

    if(pid1 == 0){
        setpriority(getpid(), 1); //high priority

        for(int i = 0; i < 5; ++i){
            printf("HIGH priority child running %d\n", i);
            busy_work();
        }

        exit(0);
    }

    pid2 = fork();

    if(pid2 == 0){
        setpriority(getpid(), 15); //low priority

        for(int i = 0; i < 5; ++i){
            printf("LOW priority child running %d\n", i);
            busy_work();
        }

        exit(0);
    }

    wait(0);
    wait(0);

    exit(0);
}