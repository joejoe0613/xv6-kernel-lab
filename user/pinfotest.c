#include "kernel/types.h"
#include "kernel/pstat.h"
#include "user/user.h"

void
busy_work(void)
{
    volatile int i;
    for(i = 0; i < 30000000; ++i)
        ;
}

char *
state_to_str(int state)
{
    switch (state)
    {
    case 0:
        return "UNUSED";
    case 1:
        return "USED";
    case 2:
        return "SLEEPING";
    case 3:
        return "RUNNABLE";
    case 4:
        return "RUNNING";
    case 5:
        return "ZOMBIE";
    
    default:
        return "UNKNOWN";
    }
}

void
print_pinfo(void)
{
    struct pstat ps;

    if(getpinfo(&ps) < 0){
        printf("getpinfo failed\n");
        return;
    }

    printf("PID   STATE       PRI   WAIT   NAME\n");
    printf("-----------------------------------\n");

    for(int i = 0; i < NPROC; ++i){
        if(ps.inuse[i]){
            printf("%d    %s    %d     %d      %s\n",
                ps.pid[i],
                state_to_str(ps.state[i]),
                ps.priority[i],
                ps.wait_ticks[i],
                ps.name[i]);
        }
    }
}

int
main(int argc, char *argv[])
{
    int pid1, pid2;

    pid1 = fork();

    if(pid1 == 0){
        setpriority(getpid(), 5);

        for(int i = 0; i < 20; ++i){
            busy_work();
        }

        exit(0);
    }

    pid2 = fork();

    if(pid2 == 0){
        setpriority(getpid(), 15);

        for(int i = 0; i < 20; ++i){
            busy_work();
        }

        exit(0);
    }

    sleep(2);
    print_pinfo();

    wait(0);
    wait(0);

    exit(0);
}