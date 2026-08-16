#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[]){
    int pid = fork();

    if(pid < 0){
        printf("fork failed\n");
        exit(1);
    }

    if(pid == 0){
        for(int i = 0; i < 5; i++){
            printf("child running %d\n", i);
            sleep(1);
        }
        exit(0);
    }
    else{
        for(int i = 0; i < 5; i++){
            printf("parent running %d\n", i);
            sleep(1);
        }
        wait(0);
    }

    exit(0);
}