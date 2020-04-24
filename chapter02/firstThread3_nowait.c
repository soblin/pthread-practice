#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct arg_t{
  int arg;
} arg_t;

void *threadWorker(void *arg){
  arg_t *ptr = (arg_t *)arg;
  const int cnt = ptr->arg;

  for(int i=0; i<cnt; ++i){
    printf("I'm in threadWorker: i = %d\n", i);
    sleep(1);
  }
  return NULL;
}

int main(int argc, char **argv){
  if(argc <= 1){
    printf("Usage: ./main <int>");
    exit(1);
  }
  const int cnt = atoi(argv[1]);
  const arg_t arg = {cnt};
  
  pthread_t thread;
  if(pthread_create(&thread, NULL, threadWorker, (void *)(&arg)) != 0){
    perror("pthread_create");
    exit(1);
  }

  for(int i=0; i<5; ++i){
    printf("I'm in main: i = %d\n", i);
    sleep(1);
  }

  printf("Bye\n");

  return 0;
}
