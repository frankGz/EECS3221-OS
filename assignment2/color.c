//  Author: Minas Spetsakis
//  Descrn: A thread performance evaluation program.
//  The two functions for the second programming component.
//  Date  :  Nov. 19 2017

#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <stdio.h>
#include <pthread.h>

#include "runner.h"
#include "proc.h"

extern int greendone, reddone;

void *redfun(void *vinfo)
{
  int i, j;
  pthread_t  my_tid;
  thread_info *info;
  int pthrerr;

  info = (thread_info *)vinfo;

  my_tid = pthread_self();
  wait2start(info);

  for (j=0; j<info->L; j++)
    {
      for (i=0; i<info->M; i++)
	{
	  blocknwrite(info, my_tid, "red  ");
	}
      pthrerr = pthread_mutex_lock(info->cntmutptr);
      if (pthrerr!=0) fatalerr("Red", pthrerr, "Locking cntmut failed\n");
      reddone++;
      if (reddone==info->N){
	greendone=0;
	pthrerr = pthread_cond_broadcast(info->greencondptr);
	if (pthrerr!=0) fatalerr("Red", pthrerr, "Broadcasting failed\n");
      }
      if (j+1<info->L){
	pthrerr = pthread_cond_wait(info->redcondptr, info->cntmutptr);
	if (pthrerr!=0) fatalerr("Red", pthrerr, "Wait on redcond failed");
      }
      pthrerr = pthread_mutex_unlock(info->cntmutptr);
      if (pthrerr!=0) fatalerr("Red", pthrerr, "Unlocking cntmut failed\n");
    }
  pthread_exit(0);
}


void *greenfun(void *vinfo)
{
  int i, j;
  pthread_t  my_tid;
  thread_info *info;
  int pthrerr;

  info = (thread_info *)vinfo;

  my_tid = pthread_self();
  wait2start(info);
  /* Here we lock the green threads until the reds have gone through once */
  pthrerr = pthread_mutex_lock(info->cntmutptr);
  if (pthrerr!=0) fatalerr("Green", pthrerr, "Locking cntmut failed\n");
  if (reddone<info->N){
    pthrerr = pthread_cond_wait(info->greencondptr, info->cntmutptr);
    if (pthrerr!=0) fatalerr("Green", pthrerr, "Wait on greencond failed");
  }
  pthrerr = pthread_mutex_unlock(info->cntmutptr);
  if (pthrerr!=0) fatalerr("Green", pthrerr, "Unlocking cntmut failed\n");

  for (j=0; j<info->L; j++)
    {
      for (i=0; i<info->M; i++)
	{
	  blocknwrite(info, my_tid, "green");
	}
      pthrerr = pthread_mutex_lock(info->cntmutptr);
      if (pthrerr!=0) fatalerr("Green", pthrerr, "Locking cntmut failed\n");
      greendone++;
      if (greendone==info->N){
	reddone=0;
	pthrerr = pthread_cond_broadcast(info->redcondptr);
	if (pthrerr!=0) fatalerr("Green", pthrerr, "Broadcasting failed\n");
      }
      if (j+1<info->L){
	pthrerr = pthread_cond_wait(info->greencondptr, info->cntmutptr);
	if (pthrerr!=0) fatalerr("Green", pthrerr, "Wait on redcond failed");
      }
      pthrerr = pthread_mutex_unlock(info->cntmutptr);
      if (pthrerr!=0) fatalerr("Green", pthrerr, "Unlocking cntmut failed\n");
    }
  pthread_exit(0);
}
