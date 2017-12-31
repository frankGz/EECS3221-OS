//  Author: Minas Spetsakis
//  Descrn: A thread performance evaluation program.
//  Routines shared between the two programming components.
//  Date  :  Nov. 19 2017

#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <stdio.h>
#include <pthread.h>

#include "runner.h"
#include "proc.h"

/************************************************************************/
/*  Blocks the thread at the start line until all threads are created   */
void wait2start(thread_info *info)
{
  int pthrerr;
  
  pthrerr = pthread_mutex_lock(info->startmutptr);
  if (pthrerr!=0) fatalerr("Runner", pthrerr, "Locking startmut failed\n");
  while (*(info->mamadoneptr)==0)
    {
      pthrerr = pthread_cond_wait(info->startlineptr, info->startmutptr);
      if (pthrerr!=0)
	fatalerr("Runner", pthrerr,"Condition startline wait failed\n");
    }
  pthrerr = pthread_mutex_unlock(info->startmutptr);
  if (pthrerr!=0)
    fatalerr("Runner", pthrerr, "Unlocking startmut failed\n");
}

/************************************************************************/
/*    Acquires mutex, writes to the file and releases mutex             */
void blocknwrite(thread_info *info, pthread_t  my_tid, char *color)
{
  int pthrerr;

  pthrerr = pthread_mutex_lock(info->filemutptr);
  if (pthrerr!=0) fatalerr("Runner", pthrerr, "Locking filemut failed\n");

  if (color==NULL){
    pthrerr = fprintf(info->f,"%lu\n",my_tid);
    if (pthrerr<=0) fatalerr("Runner", pthrerr, "fprintf failed\n");
  }
  else {
    pthrerr = fprintf(info->f,"%lu %s\n",my_tid, color);
    if (pthrerr<=0) fatalerr("Runner", pthrerr, "fprintf failed\n");
  }

  pthrerr = pthread_mutex_unlock(info->filemutptr);
  if (pthrerr!=0) fatalerr("Runner", pthrerr, "Unlocking filemut failed\n");
}
