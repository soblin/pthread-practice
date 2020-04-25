#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#define SIZE 10000000

void *threadWorker(void *arg){
  double table[SIZE];
  for(int i = 0; i<SIZE; ++i){
    table[i] = i * 3.14;
  }

  return NULL;
}

int main(int argc, char **argv){
  pthread_attr_t attr;
  pthread_t thread;

  pthread_attr_init(&attr);
  if(pthread_attr_setstacksize(&attr, SIZE * sizeof(double) + 100000) != 0){
    perror("pthread_attr_setstacksize");
  }

  if(pthread_create(&thread, &attr, threadWorker, NULL) != 0){
    perror("pthread_create");
    exit(1);
  }

  if(pthread_join(thread, NULL) != 0){
    perror("pthread_join");
    exit(1);
  }

  return 0;
}

