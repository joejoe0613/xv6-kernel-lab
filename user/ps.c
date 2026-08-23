#include "kernel/types.h"
#include "kernel/pstat.h"
#include "user/user.h"

char*
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

int
main(int argc, char *argv[])
{
    struct pstat ps;
    if(getpinfo(&ps) < 0){
        printf("getpinfo failed\n");
        exit(1);
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

    exit(0);
}