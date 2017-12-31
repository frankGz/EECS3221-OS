#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>

#include "eval.h"
#include "proc.h"

int eval(tree,inport,outport)
     evptr tree;
     int inport, outport;
{
  printtree(tree,0);
};
