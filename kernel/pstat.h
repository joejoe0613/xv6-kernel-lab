#ifndef _PSTAT_H_
#define _PSTAT_H_

#include "param.h"

struct pstat{
   int inuse[NPROC];
   int pid[NPROC];
   int state[NPROC];
   int priority[NPROC];
   int wait_ticks[NPROC];
   char name[NPROC][16];
};

#endif
