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

extern void *redfun(void *vinfo);
extern void *greenfun(void *vinfo);

extern int greendone, reddone;

int greendone, reddone;		/* These are global. There is no reason
				   to be global, other than have variety */

int main(int argc, char *argv[])
{
  pthread_t *tidr, *tidg;
  pthread_attr_t attr;
  pthread_cond_t  startline, redcond, greencond;
  pthread_mutex_t startmut, filemut, cntmut;
  thread_info *infor, *infog;
  int pthrerr, N, M, L, i;
  int mamadone;
  FILE *f;

  N=100;				/* Default number of threads */
  M=1000;				/* Default number of iterations */
  L=20;					/* Default number of alternations */

  if (argc>=2)
    N=atoi(argv[1]);
  if (argc>=3)
    M=atoi(argv[2]);
  if (argc>=4)
    L=atoi(argv[3]);

  mamadone = 0; reddone=0; greendone=0;
  f = fopen("pthread_stats","w");
  if (f==NULL)
     fatalerr(argv[0], errno,"Could not open file pthread_stats\n");
				/* Initializers never return an error code */
  pthread_cond_init(&startline, NULL);
  pthread_cond_init(&redcond, NULL);
  pthread_cond_init(&greencond, NULL);
  pthread_mutex_init(&startmut, NULL);
  pthread_mutex_init(&filemut, NULL);
  pthread_mutex_init(&cntmut, NULL);
  /* Checking or not checking these does not matter */


  infor = (thread_info*)malloc(N*sizeof(thread_info));
  if (infor==NULL) fatalerr(argv[0], errno,"Not enough memory\n");
  infog = (thread_info*)malloc(N*sizeof(thread_info));
  if (infog==NULL) fatalerr(argv[0], errno,"Not enough memory\n");
  tidr  = (pthread_t*)   malloc(N*sizeof(pthread_t));
  if (tidr==NULL) fatalerr(argv[0], errno,"Not enough memory\n");
  tidg  = (pthread_t*)   malloc(N*sizeof(pthread_t));
  if (tidg==NULL) fatalerr(argv[0], errno,"Not enough memory\n");
  pthread_attr_init(&attr);
  for (i=0; i<N; i++)
    {
      infor[i].N  = infog[i].N  = N;
      infor[i].M  = infog[i].M  = M;
      infor[i].L  = infog[i].L  = L;
      infor[i].me = infog[i].me = i;
      infor[i].f  = infog[i].f  = f;
      infor[i].startlineptr = infog[i].startlineptr = &startline;
      infor[i].redcondptr   = infog[i].redcondptr   = &redcond;
      infor[i].greencondptr = infog[i].greencondptr = &greencond;
      infor[i].startmutptr  = infog[i].startmutptr  = &startmut;
      infor[i].filemutptr   = infog[i].filemutptr   = &filemut;
      infor[i].cntmutptr    = infog[i].cntmutptr    = &cntmut;
      infor[i].mamadoneptr  = infog[i].mamadoneptr  = &mamadone;
      pthrerr = pthread_create(tidr+i, &attr, redfun,   (void*)(infor+i));
      if (pthrerr!=0) fatalerr(argv[0], pthrerr,"Thread creation failed\n");
      pthrerr = pthread_create(tidg+i, &attr, greenfun, (void*)(infog+i));
      if (pthrerr!=0) fatalerr(argv[0], pthrerr,"Thread creation failed\n");
    }
  /* This thread sets mamadone to 1 and wakes up all the blocked threads */
  pthrerr = pthread_mutex_lock(&startmut);
  if (pthrerr!=0) fatalerr(argv[0], pthrerr, "Locking startmut failed\n");
  mamadone = 1;
  pthrerr = pthread_cond_broadcast(&startline);
  if (pthrerr!=0)
    fatalerr(argv[0], pthrerr, "Broadcasting startline failed\n");
  pthrerr = pthread_mutex_unlock(&startmut);
  if (pthrerr!=0)
    fatalerr(argv[0], pthrerr, "Unlocking startmut failed\n");

  for (i=0; i<N; i++)
    {
      pthrerr = pthread_join(tidr[i],NULL);
      if (pthrerr!=0) fatalerr(argv[0], pthrerr,"Could not join red thread\n");
    }
  for (i=0; i<N; i++)
    {
      pthrerr = pthread_join(tidg[i],NULL);
      if (pthrerr!=0) fatalerr(argv[0],pthrerr,"Could not join green thread\n");
    }
  /* Here we should de-allocate and clean up the condition variables etc
     but it is not really needed */
}

