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

int main(){
  int n = 100000;
  int x = countPrimeNumbers(n);
  printf("Number of prime numbers under %d is %d\n", n, x);

  n = 200000;
  x = countPrimeNumbers(n);
  printf("Number of prime numbers under %d is %d\n", n, x);

  return 0;
}
