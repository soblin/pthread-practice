#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct arg_t {
  int val;
} arg_t;

void stopIfEqualsOne(int i){
  if(i == 1){
    printf("Hasta la vista, baby\n");
    exit(1);
  }
}
void *threadWorker(void *arg){
  arg_t *ptr = (arg_t *)arg;
  const int cnt = ptr->val;
  
  for(int i=0; i<cnt; ++i){
    printf("I'm in threadWorker: i = %d\n", i);
    sleep(1);
  }
}

int main(int argc, char **argv){
  if(argc <= 1){
    printf("Usage: ./main <int>\n");
    exit(1);
  }

  int cnt = atoi(argv[1]);
  arg_t arg = {cnt};

  pthread_t thread;

  if(pthread_create(&thread, NULL, threadWorker, (void *)(&arg)) != 0){
    perror("pthread_create");
    exit(1);
  }

  for(int i=0; i<5; ++i){
    printf("I'm in main: i = %d\n", i);
    sleep(1);
  }

  if(pthread_join(thread, NULL) != 0){
    // stop this main thread until `thread` finishes.
    perror("pthread_join");
    exit(1);
  }
  
  printf("Bye\n");
  
  return 0;
}
