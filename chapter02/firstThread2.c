#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

void stopIfEqualsOne(int i){
  if(i == 1){
    printf("Hasta la vista, baby\n");
    pthread_exit(NULL);
  }
}

void *threadFunc(void *arg){
  for(int i=0; i<3; ++i){
    printf("I'm in threadFunc: i = %d\n", i);
    stopIfEqualsOne(i); // stop if i == 1
    sleep(1);
  }
}

int main(int argc, char **argv){
  pthread_t thread;

  if(pthread_create(&thread, NULL, threadFunc, NULL) != 0){
    perror("pthread_create:failed");
    exit(1);
  }

  for(int i=0; i<5; ++i){
    printf("I'm in main: i = %d\n", i);
    sleep(1);
  }

  return 0;
}
