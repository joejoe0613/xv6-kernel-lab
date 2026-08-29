#include "kernel/types.h"
#include "user/user.h"

void
busy_work(void)
{
    volatile int i;
    for(i = 0; i < 10000000; ++i)
        ;
}

int
main(int argc, char *argv[])
{
    int sem;
    int pid;

    sem = sem_create(1);

    if(sem < 0){
        printf("sem_create failed\n");
        exit(1);
    }

    pid = fork();

    if(pid < 0){
        printf("fork failed\n");
        exit(1);
    }

    if(pid == 0){
        for(int i = 0; i < 5; ++i){
            sem_wait(sem);

            printf("CHILD enter critical sectiorn %d\n", i);
            busy_work();
            printf("CHILD leave critical sectiorn %d\n", i);

            sem_post(sem);
        }

        exit(0);
    } 
    else{

        for(int i = 0; i < 5; i++) {

        sem_wait(sem);

        printf("PARENT enter critical section %d\n", i);
        busy_work();
        printf("PARENT leave critical section %d\n", i);

        sem_post(sem);
    }

    wait(0);
  }

  exit(0);
}