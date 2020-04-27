#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N_THREAD 1000

#define MAX_PRIME_NUMBERS 100000

int primeNumber[MAX_PRIME_NUMBERS];
int nPrimeNumber;
int usingPrimeNumber;

int isPrimeNumber(int m){
  for(int i = 0; i<nPrimeNumber; ++i){
    if(m % primeNumber[i] == 0)
      return 0;
  }
  return 1;
}

void microSleep(){
  struct timespec ts;
  ts.tv_sec = 0;
  ts.tv_nsec = 10000;
  nanosleep(&ts, NULL);
}

int countPrimeNumbers(int n){
  while(usingPrimeNumber)
    microSleep();

  usingPrimeNumber = 1;
  nPrimeNumber = 0;

  for(int i = 2; i<= n; ++i){
    if(isPrimeNumber(i)){
      if(nPrimeNumber >= MAX_PRIME_NUMBERS){
        printf("Oops, too many prime numbers.\n");
        exit(1);
      }
      primeNumber[nPrimeNumber] = i;
      nPrimeNumber++;
    }
  }
  int ret = nPrimeNumber;
  usingPrimeNumber=  0;
  return ret;
}

void *threadWorker(void *arg){
  int n = (int)arg;
  int x = countPrimeNumbers(n);

  if(x != 25){
    printf("number of prime numbers under %d is %d !?\n", n, x);
  }

  return NULL;
}

int main(){
  pthread_t threads[N_THREAD];
  usingPrimeNumber = 0;

  for(int i = 0; i<N_THREAD; ++i){
    if(0 != pthread_create(&(threads[i]), NULL, threadWorker, (void *)100)){
      perror("pthread_create");
      exit(1);
    }
  }
  for(int i  = 0; i<N_THREAD; ++i){
    pthread_join(threads[i], NULL);
  }

  printf("Done\n");

  return 0;
}
