#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "eval.h"
#include "proc.h"

evptr shyyval;

int main(argc,argv)
     int argc;
     char **argv;
{
  int status, temp;
  pid_t pid;

  printf("Welcome to my little shell\n");
  printf(">> ");
  while (!yyparse())
    {
      pid = eval(shyyval,0,1);
      if (pid!=0)
	{
	  temp=waitpid(pid,&status,0);
	  if ( temp==0 )
	    {
	      printf("mysh: process terminated unexpectedly\n");
	      exit(-1);
	    }
	  else if ( temp==-1 )
	    {
	      printf("mysh: process terminated unexpectedly: %s\n",
		     strerror(errno));
	      exit(-1);
	    }
	}
      printf(">> ");
    }
  printf("please learn how to type correctly and try again!\n");
}

