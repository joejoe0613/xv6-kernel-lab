#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
    int q;
    int producer;
    int consumer;

    q = qcreate();

    if(q < 0){
        printf("qcreate failed\n");
        exit(1);
    }

    producer = fork();

    if(producer < 0){
        printf("producer fork failed\n");
        exit(1);
    }

    if(producer == 0){
        for(int i = 0; i < 10; ++i){            
            qsend(q, i);
            
            printf("producer sending %d\n", i);
        }

        exit(0);
    }

    consumer = fork();

    if(consumer < 0){
        printf("consumer fork failed\n");
        exit(1);
    }

    if(consumer == 0){
        sleep(10);

        for(int i = 0; i < 10; ++i){
            int value;

            qrecv(q, &value);

            printf("consumer received %d\n", value);

            sleep(2);
        }

        exit(0);
    }

    wait(0);
    wait(0);

    printf("queue test finished\n");

    exit(0);
}