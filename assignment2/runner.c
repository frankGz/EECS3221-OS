//  Author: Minas Spetsakis
//  Descrn: A thread performance evaluation program.
//  The functions for the first programming component.
//  Date  :  Nov. 19 2017

#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <stdio.h>
#include <pthread.h>

#include "runner.h"
#include "proc.h"

void *runner(void *vinfo)
{
  int i;
  pthread_t  my_tid;
  thread_info *info;
  int pthrerr;

  info = (thread_info *)vinfo;

  my_tid = pthread_self();
  wait2start(info);
  
  for (i=0; i<info->M; i++)
    {				/* Moved the ugly bulk to util.c */
      blocknwrite(info, my_tid, NULL);
    }
  pthread_exit(0);
}

