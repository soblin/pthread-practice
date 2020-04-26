#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char *varA;
#define STRINGSIZE (32)

void *threadWorker(void *arg){
  int n = (int)arg;
  snprintf(varA, STRINGSIZE, "Hello I'm No.%d", n);
  printf("threadWorker-%d, sets varA as %s\n", n, varA);
  sleep(2);
  printf("threadWorker-%d, sets varA as %s\n", n, varA);

  return NULL;
}

int main(void){
  pthread_t thread1, thread2;

  char varB[STRINGSIZE];

  varA = varB;

  strcpy(varA, "Good Morning");
  printf("main-1: varA is %s, varB is %s\n", varA, varB);
  if(0 != pthread_create(&thread1, NULL, threadWorker, (void *)1)){
    perror("pthread_create1");
    exit(1);
  }
  sleep(1);
  printf("main-2: varA is %s, varB is %s\n", varA, varB);
  if(0 != pthread_create(&thread2, NULL, threadWorker, (void *)2)){
    perror("pthread_create");
    exit(1);
  }
  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);
  printf("main-3: varA is %s, varB is %s\n", varA, varB);

  return 0;
}
