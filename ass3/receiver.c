#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/signal.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

/* the name of the shared file and the size */
#define SHARED_FILE "sharedfile"
#define FILE_SIZE   100

/* mark which signal has just happend.
 * This is used to let the signal hander notify the main function
 * to print the content in the mmap array or to terminate. */
static unsigned sigusr1 = 0;
static unsigned sigterm = 0;

void sig_handler(int signo) {
  if (signo == SIGUSR1) {
    sigusr1 = 1;
  }
  if (signo == SIGTERM) {
    sigterm = 1;
  }
}

int main() {
  int fid;
  char *arr; /* the array to map the file into */
  unsigned count;
  struct sigaction act;
  sigset_t mask, omask;

  printf("receiver pid=%d. receiving...\n\n", getpid());

  /* map the file in read mode into an array using mmap */
  fid = open(SHARED_FILE, O_RDONLY);
  if (fid < 0) {
    fprintf(stderr, "could not open file %s to read [%d]\n", SHARED_FILE, errno);
    exit(errno);
  }
  arr = mmap(NULL, FILE_SIZE, PROT_READ, MAP_SHARED, fid, 0);
  if (arr == MAP_FAILED) {
    fprintf(stderr, "could not map file %s [%d]\n", SHARED_FILE, errno);
    exit(errno);
  }
  close(fid);

  /* setup the SIGUSR1 and SIGTERM handler to receive notification
   * from the sender and terminate gracefully */
  memset(&act, 0, sizeof(act));
  act.sa_handler = sig_handler;
  if (sigaction(SIGUSR1, &act, NULL) < 0 || sigaction(SIGTERM, &act, NULL) < 0) {
    fprintf(stderr, "could not create signal handler [%d]\n", errno);
    exit(errno);
  }

  /* read until the reader send SIGTERM */
  count = 0;
  sigemptyset(&mask);
  sigaddset(&mask, SIGUSR1);
  sigaddset(&mask, SIGTERM);
  sigprocmask(SIG_BLOCK, &mask, &omask);
  while (1) {
    if(sigusr1 > 0) {
      /* print the characters added into the array */
      while (count < FILE_SIZE && arr[count] != 0) {
        printf("%c", arr[count]);
        count ++;
      }
      sigusr1 = 0;
    }
    if (sigterm > 0) {
      /* the user wants to quit the program, quit gracefully */
      sigterm = 0;
      break;
    }
    sigsuspend(&omask);
  }

  /* quit the program gracefully */
  fflush(stdout);
  printf("\n\nsender is done. quitting gracefully..\n");
  munmap(arr, FILE_SIZE);
  sigprocmask(SIG_UNBLOCK, &mask, NULL);
  return 0;
}

