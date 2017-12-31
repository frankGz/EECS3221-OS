#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#include "eval.h"
#include "proc.h"
extern int sfork(); /* Should have gone in proc.h */

int evalor(evptr tree, int inport, int outport);
int evaland(evptr tree, int inport, int outport);
int evalsyn(evptr tree, int inport, int outport);
int evalasyn(evptr tree, int inport, int outport);
int evalpipe(evptr tree, int inport, int outport);
int evalredin(evptr tree, int inport, int outport);
int evalredout(evptr tree, int inport, int outport);
int evalsimple(evptr tree, int inport, int outport);
int evalcd(evptr tree, int inport, int outport);
int evalnull(evptr tree, int inport, int outport);
int evalexit(evptr tree, int inport, int outport);


int eval(evptr tree,int inport,int outport)
{
  switch (tree->tag)
    {
    case ORTAG:
      return evalor(tree,inport,outport);
    case ANDTAG:
      return evaland(tree,inport,outport);
    case LISTAG:
      return evalsyn(tree,inport,outport);
    case ASYNCHTAG:
      return evalasyn(tree,inport,outport);
    case SYNCHTAG:
      return evalsyn(tree,inport,outport);
    case PIPETAG:
      return evalpipe(tree,inport,outport);
    case REDINTAG:
      return evalredin(tree,inport,outport);
    case REDOUTAG:
      return evalredout(tree,inport,outport);
    case SIMPLETAG:
      return evalsimple(tree,inport,outport);
    case CDTAG:
      return evalcd(tree,inport,outport);
    case NULLTAG:
      return evalnull(tree,inport,outport);
    case EXITAG:
      return evalexit(tree,inport,outport);
    }
}

int evalcd(evptr tree,int inport,int outport)
{
  if ( chdir(tree->leaf.data->next->fname) < 0 )
    {
      printf("mysh: Cannot change directory\n");
      exit(-1);
    }
  /*  fprintf(stderr,"In evalcd %s\n",tree->leaf.data->next->fname);*/
  return 0;
}

int evalnull(evptr tree,int inport,int outport)
{
  return 0;
}

int evalpipe(evptr tree,int inport,int outport)
{
  int p[2];
  pid_t pid;

  if (pipe(p)==-1)
    {
      printf("mysh: could not open file\n");
      exit(-1);
    }
  eval(tree->son.left,inport,p[1]);
  
  pid = eval(tree->son.right,p[0],outport);
  close(p[0]);
  close(p[1]);
  return pid;
}

int evalredin(evptr tree,int inport,int outport)
{
  int fd;
  pid_t pid;

  fd = open(tree->son.right->leaf.data->fname,O_RDONLY);
  if (fd==-1)
    {
      printf("mysh: Cannot open file\n");
      exit(-1);
    }
  pid = eval(tree->son.left,fd,outport);
  close(fd);
  return pid;
}


int evalredout(evptr tree,int inport,int outport)
{
  int fd;
  pid_t pid;

  fd = open(tree->son.right->leaf.data->fname,O_WRONLY|O_TRUNC|O_CREAT,0777);
  if (fd==-1)
    {
      printf("mysh: Cannot open file\n");
      exit(-1);
    }
  pid = eval(tree->son.left,inport,fd);
  close(fd);
  return pid;
}

int evalsyn(evptr tree,int inport,int outport)
{
  int temp, status;
  pid_t pid;

  pid = eval(tree->son.left,inport,outport);
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
  return eval(tree->son.right,inport,outport);
}


int evalasyn(evptr tree,int inport,int outport)
{
  eval(tree->son.left,inport,outport);
  return eval(tree->son.right,inport,outport);
}

int evalor(evptr tree,int inport,int outport)
{
  int status, temp;
  pid_t pid;

  pid = eval(tree->son.left,inport,outport);
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
  else
    status = 0;
  if (WEXITSTATUS(status) != 0) return eval(tree->son.right,inport,outport);
  return 0;
}

int evaland(evptr tree,int inport,int outport)
{
  int status, temp;
  pid_t pid;

  pid = eval(tree->son.left,inport,outport);
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
  else
    status = 0;
  if (WEXITSTATUS(status) == 0) return eval(tree->son.right,inport,outport);
  return 0;
}

int evalexit(evptr tree,int inport,int outport)
{
  exit(0);
}

int evalsimple(evptr tree,int inport,int outport)
{
  int i;
  pid_t st;
  struct stringlist *scan;
  char **argv, *name, *temp;

  if (0 == ( st=fork() ))
    {
      if (inport != 0)
	{
	  close(0);
	  dup(inport);
	  close(inport);
	}
      if (outport != 1)
	{
	  close(1);
	  dup(outport);
	  close(outport);
	}
      for (i=3; i<10; i++) close(i);
      i=0;			/* Count the arguments */
      for (scan=tree->leaf.data; scan; scan=scan->next)
	i++;
				/* allocate vector */
      argv = (char**) malloc((i+1)*sizeof(char*));
      if (!argv)
	{
	  printf("mysh: Not enough memory\n");
	  exit(-1);
	};
      i=0;			/* Fill vector with arguments */
      for (scan=tree->leaf.data; scan; scan=scan->next)
	argv[i++] = scan->fname;
      argv[i] = NULL;
      temp = name = *argv;	/* Set the name of the file properly */
      while (temp = strchr(temp,'/'))
	*argv = temp;
      execvp(name, argv);
				/* Exec has failed! */
      printf("mysh: exec failed\n");
      exit(-1);
    }
  if (st==-1)
    {
      printf("mysh: Fork failed\n");
      exit(-1);
    }
  return st;
};
