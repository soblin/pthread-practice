#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_PRIME_NUMBERS 100000

int primeNumber[MAX_PRIME_NUMBERS];
int nPrimeNumber;
int usingPrimeNumber;

int isPrimeNumber(int m){
  for(int i = 0; i < nPrimeNumber; ++i){
    if(m % primeNumber[i] == 0)
      return 0;
  }
  return 1;
}

void microSleep(){
  struct timespec ts;
  ts.tv_sec = 0;
  ts.tv_nsec = 1000;
  nanosleep(&ts, NULL);
}

int countPrimeNumbers(int n){
  while(usingPrimeNumber)
    microSleep();
  // if this while ends, usingPrimeNumber == 0, so lock it
  usingPrimeNumber = 1;
  nPrimeNumber = 0;
  
  for(int i = 2; i <= n; ++i){
    if(isPrimeNumber(i)){
      if(nPrimeNumber >= MAX_PRIME_NUMBERS){
        printf("Oops, too many prime numbers\n");
        exit(1);
      }
      primeNumber[nPrimeNumber] = i;
      nPrimeNumber++;
    }
  }
  int ret = nPrimeNumber;
  usingPrimeNumber = 0;
  return ret;
}

void *threadWorker(void *arg){
  int n = (int)arg;

  int x = countPrimeNumbers(n);
  printf("number of prime numbers under %d is %d\n", n, x);

  return NULL;
}

int main(void){
  pthread_t thread1, thread2;

  usingPrimeNumber = 0;

  if(0 != pthread_create(&thread1, NULL, threadWorker, (void *)100000)){
    perror("pthread_create");
    exit(1);
  }
  if(0 != pthread_create(&thread2, NULL, threadWorker, (void *)200000)){
    perror("pthread_create");
    exit(1);
  }

  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);

  return 0;
}
