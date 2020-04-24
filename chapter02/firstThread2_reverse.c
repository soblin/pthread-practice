#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void *threadWorker(void *arg){
  for(int i=0; i<5; ++i){
    printf("I'm in thread: i = %d\n", i);
    sleep(1);
  }
  return NULL;
}

int main(int argc, char **argv){
  pthread_t thread;

  if(pthread_create(&thread, NULL, threadWorker, NULL) != 0){
    perror("pthread_create");
    exit(1);
  }

  for(int i=0; i<3; ++i){
    printf("I'm in main thread: i = %d\n", i);
    sleep(1);
  }

  return 0;
}
