#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void stopIfEqualsOne(int i){
  if(i == 1){
    printf("Hasta la vista, baby\n");
    exit(1);
  }
}
void *threadWorker(void *arg){
  for(int i=0; i<3; ++i){
    printf("I'm in threadWorker: i = %d\n", i);
    stopIfEqualsOne(i);
    sleep(1);
  }
}

int main(int argc, char **argv){
  pthread_t thread;

  if(pthread_create(&thread, NULL, threadWorker, NULL) != 0){
    perror("pthread_create");
    exit(1);
  }

  for(int i=0; i<5; ++i){
    printf("I'm in main: i = %d\n", i);
    sleep(1);
  }

  return 0;

