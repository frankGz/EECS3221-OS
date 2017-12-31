#include <unistd.h>
#include <stdio.h>
#include <sys/signal.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

/* the name of the shared file and the size */
#define SHARED_FILE "sharedfile"
#define FILE_SIZE   100

int main(int argc, char *argv[]) {
  pid_t receiver;
  int fid;
  char *arr; /* the array to map the file into */
  unsigned count;

  /* get the PID of the receiver from the argument */
  if (argc != 2) {
    fprintf(stderr, "Usage: %s receiver_pid\n", argv[0]);
    exit(-1);
  }

  receiver = (pid_t) atoi(argv[1]);

  /* open the shared file and mapped into an array using mmap */
  fid = open(SHARED_FILE, O_RDWR);
  if (fid < 0) {
    fprintf(stderr, "could not open file %s to write [%d]\n", SHARED_FILE, errno);
    exit(errno);
  }

  arr = mmap(NULL, FILE_SIZE, PROT_WRITE, MAP_SHARED, fid, 0);
  if (arr == MAP_FAILED) {
    fprintf(stderr, "could not map file %s [%d]\n", SHARED_FILE, errno);
    exit(errno);
  }
  close(fid);

  /* clean the file before sending anything */
  memset(arr, 0, FILE_SIZE);

  /* read user's input and send it to the array.
   * send SIGUSR1 to receiver every time.
   * When the array is full, send SIGTERM to the receiver. */
  count = 0;
  printf("Enter up to %d characters (including \\n):\n", FILE_SIZE);
  while (count < FILE_SIZE) {
    int ch = fgetc(stdin);
    if (ch == EOF) { /* user terminates the input before the limit */
      break;
    }
    /* save the character in the array and notify the receiver */
    arr[count] = ch;
    count ++;
    if (kill(receiver, SIGUSR1) < 0) {
      fprintf(stderr, "could not send SIGUSR1 to receiver %d [%d]\n", receiver, errno);
      break;
    }
  }
  /* the array is full, terminate after cleanning up the mapping */
  kill(receiver, SIGTERM);
  munmap(arr, FILE_SIZE);

  return 0;
}

