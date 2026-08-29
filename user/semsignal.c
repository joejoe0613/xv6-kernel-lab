#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int event_sem;
  int ready_sem;
  int pid;

  event_sem = sem_create(0);
  ready_sem = sem_create(0);

  if(event_sem < 0 || ready_sem < 0) {
    printf("sem_create failed\n");
    exit(1);
  }

  pid = fork();

  if(pid < 0) {
    printf("fork failed\n");
    exit(1);
  }

  if(pid == 0) {

    printf("child: waiting for event...\n");

    // 告訴 parent：我已經準備要等待 event 了
    sem_post(ready_sem);

    // 這裡一定會 block，因為 event_sem 初始值是 0
    sem_wait(event_sem);

    printf("child: received event!\n");

    exit(0);

  } else {

    // parent 一定等到 child 已經走到前面的同步點
    sem_wait(ready_sem);

    printf("parent: sending event\n");

    sem_post(event_sem);

    wait(0);
  }

  exit(0);
}