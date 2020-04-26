#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

void *threadWorker(void *arg){
  const char *msg = "threadFunc writes to standard output\n";
  write(1, msg, strlen(msg));
  close(1);
  open("fdThread.txt", O_WRONLY | O_CREAT | O_TRUNC, 0744);
  msg = "threadFunc writes to fdThread.txt\n";
  write(1, msg, strlen(msg));
  sleep(2);
  pthread_exit(NULL);
}

int main(void){
  pthread_t thread;

  pthread_create(&thread, NULL, threadWorker, NULL);
  sleep(1);

  const char *msg = "main writes to standard output\n";
  // fd`1` points to `fdThread.txt`, which is left open in threadWorker
  write(1, msg, strlen(msg));

  pthread_join(thread, NULL);

  return 0;
}
