#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "eval.h"
#include "proc.h"


/**********************************************************************
		mk_evtree
***********************************************************************/

evptr mk_evtree(tag,left,right)
     int tag;
     void *left, *right;
{
  evptr res;
				/* returns a newly allocated node of    */
				/* an evtree. If the tag corresponds    */
				/* to a binary node both arguments are  */
				/* used. Accordingly for unary and leaf */
				/* nodes one or zero extra arguments    */
				/* are needed                           */
  res = (evptr) malloc(sizeof(union evtree));
  if (!res)
    {
      printf("mk_evtree: Not enough memory\n");
      exit(-1);
    };
  res->tag = tag;
  switch (tag)
    {
    case ORTAG:
    case ANDTAG:
    case LISTAG:
    case ASYNCHTAG:
    case SYNCHTAG:
    case PIPETAG:
    case REDINTAG:
    case REDOUTAG:
      res->son.left=left;
      res->son.right=right;
      return res;
    case SIMPLETAG:
    case CDTAG:
      res->leaf.data=left;
      return res;
    case NULLTAG:
    case EXITAG:
      return res;
    };
  return res;
}

/**********************************************************************
		mk_stringlist
***********************************************************************/

struct stringlist *mk_stringlist(str,nxt)
     char *str;
     struct stringlist *nxt;
{
  struct stringlist *res;
  char *tmpstr;
				/* return a newly allocated node         */
				/* of a string list that contains a copy */
				/* of str and nxt                        */
  res = (struct stringlist*) malloc(sizeof(struct stringlist));
  if (!res)
    {
      printf("mk_stringlist: Not enough memory\n");
      exit(-1);
    };

  tmpstr = (char*) malloc(strlen(str)+1);
  if (!tmpstr)
    {
      printf("mk_stringlist: Not enough memory\n");
      exit(-1);
    };
  strcpy(tmpstr, str);
  res->fname = tmpstr;
  res->next = nxt;
  return res;
};

/***********************************************************************
		printtree
************************************************************************/

void printtree(tree,depth)
     evptr tree;
     int depth;
{
  char *s;
  struct stringlist *scan;
  int i;
				/* Print the evtree sideways for debugging */
				/* purposes.                               */
				/* Most routines that accept an evtree     */
				/* as argument have this form              */
  switch (tree->tag)
    {
    case ORTAG:
      s = "||";
      goto printit;
    case ANDTAG:
      s = "&&";
      goto printit;
    case LISTAG:
      s = ";";
      goto printit;
    case ASYNCHTAG:
      s = "&";
      goto printit;
    case SYNCHTAG:
      s = ";";
      goto printit;
    case PIPETAG:
      s = "|";
      goto printit;
    case REDINTAG:
      s = "<";
      goto printit;
    case REDOUTAG:
      s = ">";
      goto printit;
    printit:
      printtree(tree->son.right,depth+1);
      for (i=0; i<depth; i++) printf("   ");
      printf("%s\n",s);
      printtree(tree->son.left,depth+1);
      break;
    case SIMPLETAG:
    case CDTAG:
      for (i=0; i<depth; i++) printf("   ");
      for (scan = tree->leaf.data; scan != NULL; scan = scan->next)
	printf("%s ",scan->fname);
      printf("\n");
      break;
    case NULLTAG:
      for (i=0; i<depth; i++) printf("   ");
      printf("<NULL>\n");
      break;
    case EXITAG:
      printf("exit\n");
      break;
    default:
      printf("Unknown tag %d\n",tree->tag);
      break;
    };
};
