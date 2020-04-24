#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void *threadFunc(void *arg){
  int i;

  for(i=0; i<3; i++){
    printf("I'm threadFunc: i = %d\n", i);
    sleep(1);
  }

  return NULL;
}

int main(int argc, char **argv){
  pthread_t thread;
  int i;

  if(pthread_create(&thread, NULL, threadFunc, NULL) != 0){
    perror("pthread_create: failed");
    exit(1);
  }

  for(i=0; i<5; i++){
    printf("I'm main thread: i = %d\n", i);
    sleep(1);
  }

  return 0;
}
