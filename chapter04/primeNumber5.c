#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define N_THREAD 1000

#define MAX_PRIME_NUMBERS 100000

int primeNumber[MAX_PRIME_NUMBERS];
int nPrimeNumber;
pthread_mutex_t usingPrimeNumber;

int isPrimeNumber(int m){
  for(int i = 0; i<nPrimeNumber; ++i){
    if(m % primeNumber[i] ==0 )
      return 0;
  }
  return 1;
}

int countPrimeNumbers(int n){
  // while usingPrimeNumber is used by others, I stop here
  pthread_mutex_lock(&usingPrimeNumber);

  nPrimeNumber = 0;
  for(int i = 2; i<= n; ++i){
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
  // I release usingPrimeNumbers
  pthread_mutex_unlock(&usingPrimeNumber);
  return ret;
}

void *threadFunc(void *arg){
  int n = (int)arg;
  int x = countPrimeNumbers(n);

  if(x != 25){
    printf("number of prime numbers under %d is %d !?\n", x, n);
  }

  return NULL;
}

int main(){
  pthread_t threads[N_THREAD];
  pthread_mutex_init(&usingPrimeNumber, NULL);

  for(int i = 0; i<N_THREAD; ++i){
    if(0 != pthread_create(&threads[i], NULL, threadFunc, (void *)100)){
      perror("pthread_create");
      exit(1);
    }
  }

  for(int i = 0; i<N_THREAD; ++i){
    pthread_join(threads[i], NULL);
  }

  pthread_mutex_destroy(&usingPrimeNumber);

  printf("Done\n");

  return 0;
}
