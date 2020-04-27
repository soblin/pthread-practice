#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_PRIME_NUMBERS 100000

int primeNumber[MAX_PRIME_NUMBERS];
int nPrimeNumber;

int isPrimeNumber(int m){
  for(int i = 0; i<nPrimeNumber; ++i){
    if(m % primeNumber[i] == 0)
      return 0;
  }
  return 1;
}

int countPrimeNumbers(int n){
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
  return nPrimeNumber;
}

void *threadFunc(void *arg){
  int n = (int)arg;
  int x = countPrimeNumbers(n);
  printf("number of prime numbers under %d is %d\n", n, x);

  return NULL;
}

int main(){
  pthread_t thread1, thread2;

  if(0 != pthread_create(&thread1, NULL, threadFunc, (void *)100000)){
    perror("pthread_create");
    exit(1);
  }
  if(0 != pthread_create(&thread2, NULL, threadFunc, (void *)200000)){
    perror("pthread_create");
    exit(1);
  }
  if(0 != pthread_join(thread1, NULL)){
    perror("pthread_join");
    exit(1);
  }
  if(0 != pthread_join(thread2, NULL)){
    perror("pthread_join");
    exit(1);
  }

  return 0;
}
