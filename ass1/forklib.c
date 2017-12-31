#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int forkdepth = 10;

extern int sfork()
{
  if (--forkdepth >0)
    return fork();
  fprintf(stderr,"Too many forks\n");
  exit(-1);
}

