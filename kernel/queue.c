#include "types.h"
#include "param.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"

#define NQUEUE 8
#define QSIZE 4

struct kqueue
{
    struct spinlock lock;

    int used;

    int buffer[QSIZE];

    int head;
    int tail;
    int count;

    int not_empty;
    int not_full;
};


static struct kqueue queues[NQUEUE];

void
queueinit(void)
{
    int i;

    for(i = 0; i < NQUEUE; ++i){
        initlock(&queues[i].lock, "queue");

        queues[i].used = 0;
        queues[i].head = 0;
        queues[i].tail = 0;
        queues[i].count = 0;
    }
}

int
qcreate(void)
{
    int i;

    for(i = 0; i < NQUEUE; ++i){
        acquire(&queues[i].lock);

        if(queues[i].used == 0){
            queues[i].used = 1;
            queues[i].head = 0;
            queues[i].tail = 0;
            queues[i].count = 0;

            release(&queues[i].lock);

            return i;
        }

        release(&queues[i].lock);
    }

    return -1;
}

int
qsend_kernel(int id, int value)
{
    struct kqueue *q;

    if(id < 0 || id >= NQUEUE)
        return -1;
    
    q = &queues[id];

    acquire(&q->lock);

    if(q->used == 0){
        release(&q->lock);
        return -1;
    }

    while(q->count == QSIZE){
        //printk("qsend: pid %d BLOCKED, count = %d\n", myproc()->pid, q->count);

        sleep(&q->not_full, &q->lock);

        //printk("qsend: pid %d WAKEUP, count = %d\n", myproc()->pid, q->count);
    }

    q->buffer[q->tail] = value;
    q->tail = (q->tail + 1) % QSIZE;
    q->count++;

    //printk("qsend: value=%d count=%d\n", value, q->count);

    wakeup(&q->not_empty);

    release(&q->lock);

    return 0;
}

int
qrecv_kernel(int id, int *value)
{
    struct kqueue *q;

    if(id < 0 || id >= NQUEUE)
        return -1;
    
    q = &queues[id];

    acquire(&q->lock);

    if(q->used == 0){
        release(&q->lock);
        return -1;
    }

    while(q->count == 0){
        //printk("qrecv: pid %d BLOCKED, count = %d\n", myproc()->pid, q->count);

        sleep(&q->not_empty, &q->lock);

        //printk("qrecv: pid %d WAKEUP, count = %d\n", myproc()->pid, q->count);
    }

    *value = q->buffer[q->head];
    q->head = (q->head + 1)% QSIZE;
    q->count--;

    //printk("qrecv: value=%d count=%d\n", *value, q->count);

    wakeup(&q->not_full);

    release(&q->lock);

    return 0;
}