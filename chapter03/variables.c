#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int varA;

void *threadWorker(void *arg){
  int n = (int)arg;

  int varB;

  varB = 4 * n;
  printf("threadWorker-%d-1: varA = %d, varB = %d\n", n, varA, varB);
  varA = 5 * n;
  printf("threadWorker-%d-2: varA = %d, varB = %d\n", n, varA, varB);
  sleep(2);
  printf("threadWorker-%d-3: varA = %d, varB = %d\n", n, varA, varB);
  varB = 6 * n;
  printf("threadWorker-%d-4: varA = %d, varB = %d\n", n, varA, varB);

  return NULL;
}

int main(void){
  pthread_t thread1, thread2;
  int varB;

  varA = 1; varB = 2;
  printf("main-1: varA = %d, varB = %d\n", varA, varB);
  if(0 != pthread_create(&thread1, NULL, threadWorker, (void *)1)){
    perror("pthread_create1");
    exit(1);
  }
  sleep(1);
  varB = 3;
  printf("main-2: varA = %d, varB = %d\n", varA, varB);
  if(0 != pthread_create(&thread2, NULL, threadWorker, (void *)2)){
    perror("pthread_create2");
    exit(1);
  }
  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);

  printf("main-3: varA = %d, varB = %d\n", varA, varB);

  return 0;
}
