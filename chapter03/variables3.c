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
  printf("threadWorker-%d: Sets varA as %s\n", n, varA);
  sleep(2);

  printf("threadWorker-%d: After 2 secs. varA is %s\n", n, varA);

  return NULL;
}

int main(void){
  pthread_t thread1, thread2;

  varA = (char *)malloc(STRINGSIZE);
  strcpy(varA, "Good morning");
  printf("main-1: varA is %s\n", varA);
  if(0 != pthread_create(&thread1, NULL, threadWorker, (void *)1)){
    perror("pthread_create1");
    exit(1);
  }
  sleep(1);
  if(0 != pthread_create(&thread2, NULL, threadWorker, (void *)2)){
    perror("pthread_create2");
    exit(1);
  }
  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);
  printf("main-3: varA is %s\n", varA);

  return 0;
}
