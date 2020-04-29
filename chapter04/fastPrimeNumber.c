#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_PRIME_NUMBERS 100000

int primeNumber[MAX_PRIME_NUMBERS];
int nPrimeNumber;

// そこまでの素数を全て確認しているという数
int primeNumberChecked;
pthread_mutex_t primeNumberCheckedMutex;

// mが素数かどうか判定する．
// m/2以下の素数が全てprimeNumberに入っているとする
int isPrimeNumber(int m){
  for(int i = 0; i<nPrimeNumber; ++i){
    if(primeNumber[i] > m/2)
      return 1;
    if(m % primeNumber[i] == 0)
      return 0;
  }
  return 1;
}

void generatePrimeNumbers(int n){
  pthread_mutex_lock(&primeNumberCheckedMutex);
  
  if(n <= primeNumberChecked){
    pthread_mutex_unlock(&primeNumberCheckedMutex);
    return;
  }
  for(int i = primeNumberChecked + 1; i <= n; ++i){
    if(isPrimeNumber(i)){
      if(nPrimeNumber >= MAX_PRIME_NUMBERS){
        printf("Oops, too many prime numbers");
        exit(1);
      }
      primeNumber[nPrimeNumber] = i;
      nPrimeNumber++;
    }
  }
  primeNumberChecked = n;
  pthread_mutex_unlock(&primeNumberCheckedMutex);
  
  return;
}

int countPrimeNumbers(int n){
  generatePrimeNumbers(n);
  int count = 0;
  for(int i = 0; i<nPrimeNumber; ++i){
    if(primeNumber[i] > n)
      break;
    count++;
  }
  return count;
}

void *threadWorker(void *arg){
  int n = (int)arg;
  int x = countPrimeNumbers(n);
  printf("Number of prime numbers under %d is %d\n", n, x);

  return NULL;
}

int main(){
  int numberList[6] = {1, 10, 100, 1000, 10000, 100000};
  pthread_t threads[6];

  nPrimeNumber = 0;
  primeNumberChecked = 1;
  pthread_mutex_init(&primeNumberCheckedMutex, NULL);
  
  for(int i = 0; i<0; ++i){
    if(0 != pthread_create(&threads[i], NULL, threadWorker, (void *)numberList[i])){
      perror("pthread_create");
      exit(1);
    }
  }
  for(int i = 0; i<6; ++i){
    threadWorker((void *)numberList[i]);
  }
  for(int i = 0; i<0; ++i){
    pthread_join(threads[i], NULL);
  }

  pthread_mutex_destroy(&primeNumberCheckedMutex);
  printf("Done\n");

  return 0;
}
