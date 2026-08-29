#include "types.h"
#include "param.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"

#define NSEM 16

struct ksem{
    struct spinlock lock;
    int used;
    int value;
};

static struct ksem sems[NSEM];

void
seminit(void)
{
    int i;

    for(i = 0; i < NSEM; ++i){
        initlock(&sems[i].lock, "semaphore");
        sems[i].used = 0;
        sems[i].value = 0;
    }
}

int
sem_create(int value)
{
    int i;

    if(value < 0)
        return -1;

    for(i = 0; i < NSEM; ++i){
        acquire(&sems[i].lock);

        if(sems[i].used == 0){
            sems[i].used = 1;
            sems[i].value = value;

            release(&sems[i].lock);
            return i;
        }

        release(&sems[i].lock);
    }

    return -1;
}

int
sem_wait_kernel(int id)
{
    if(id < 0 || id >= NSEM)
        return -1;

    acquire(&sems[id].lock);

    if(sems[id].used == 0){
        release(&sems[id].lock);
        return -1;
    }

    while(sems[id].value == 0){
        sleep(&sems[id], &sems[id].lock);
    }

    sems[id].value--;

    release(&sems[id].lock);

    return 0;
}

int
sem_post_kernel(int id)
{
    if(id < 0 || id >= NSEM)
        return -1;

    acquire(&sems[id].lock);

    if(sems[id].used == 0){
        release(&sems[id].lock);
        return -1;
    }

    sems[id].value++;

    wakeup(&sems[id]);

    release(&sems[id].lock);
    
    return 0;
}