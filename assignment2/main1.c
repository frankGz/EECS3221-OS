//  Author: Minas Spetsakis
//  Descrn: A thread performance evaluation program.
//  Date  :  Nov. 19 2017

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#include "runner.h"
#include "proc.h"

extern void *runner(void *vinfo);

int main(int argc, char *argv[])
{
  pthread_t *tid;
  pthread_attr_t attr;
  pthread_cond_t  startline;
  pthread_mutex_t startmut, filemut;
  thread_info *info;
  int pthrerr, N, M, i;
  int mamadone;			/* Shared variable is not global */
  FILE *f;

  N=100;			/* Default number of threads */
  M=1000;			/* Default number of iterations */

  if (argc>=2)
    N=atoi(argv[1]);
  if (argc==3)
    M=atoi(argv[2]);

  mamadone = 0;
  f = fopen("pthread_stats","w");
  if (f==NULL)
     fatalerr(argv[0], errno,"Could not open file pthread_stats\n");
  pthrerr = pthread_cond_init(&startline, NULL);
  if (pthrerr!=0)		/* Initializers never return an error code */
    fatalerr(argv[0], 0,"Initialization failed\n");
  pthrerr = pthread_mutex_init(&startmut, NULL);
  if (pthrerr!=0)
    fatalerr(argv[0], 0,"Initialization failed\n");
  pthrerr = pthread_mutex_init(&filemut, NULL);
  if (pthrerr!=0)
    fatalerr(argv[0], 0,"Initialization failed\n");

  info = (thread_info*)malloc(N*sizeof(thread_info));
  if (info==NULL)
    fatalerr(argv[0], errno,"Not enough memory\n");
  tid  = (pthread_t*)   malloc(N*sizeof(tid));
  if (tid==NULL)
    fatalerr(argv[0], errno,"Not enough memory\n");
  pthread_attr_init(&attr);
  for (i=0; i<N; i++)
    {
      info[i].N  = N;
      info[i].M  = M;
      info[i].me = i;
      info[i].f  = f;
      info[i].startlineptr = &startline;
      info[i].startmutptr  = &startmut;
      info[i].filemutptr   = &filemut;
      info[i].mamadoneptr  = &mamadone;
      pthrerr = pthread_create(tid+i, &attr, runner, (void*)(info+i));
      if (pthrerr!=0)
	fatalerr(argv[0], pthrerr,"Thread creation failed\n");
    }

  /*printf("I am mama thread and my PID is %d\n",getpid());*/

  pthrerr = pthread_mutex_lock(&startmut);
  if (pthrerr!=0)
    fatalerr(argv[0], pthrerr, "Locking startmut failed\n");
  mamadone = 1;
  pthrerr = pthread_cond_broadcast(&startline);
  if (pthrerr!=0)
    fatalerr(argv[0], pthrerr, "Broadcasting startline failed\n");
  pthrerr = pthread_mutex_unlock(&startmut);
  if (pthrerr!=0)
    fatalerr(argv[0], pthrerr, "Unlocking startmut failed\n");

  for (i=0; i<N; i++)
    pthread_join(tid[i],NULL);
}

