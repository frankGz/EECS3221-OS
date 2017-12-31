
typedef struct thread_info thread_info;

struct thread_info
{
  int N;				     /* Number of threads */
  int M;				     /* Number of iterations */
  int L;				     /* Number of alternations */
  int me;
  FILE *f;
  pthread_cond_t  *startlineptr;	     /* These are pointers */
  pthread_cond_t  *redcondptr, *greencondptr;
  pthread_mutex_t *startmutptr, *filemutptr; /* These are pointers */
  pthread_mutex_t *cntmutptr;
  int *mamadoneptr;			     /* to avoid global variables */
};


