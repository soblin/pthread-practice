#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define DATASIZE (10000000)
#define THREADS (10)

int data[DATASIZE];

void setRandomData(){
  for(int i = 0; i<DATASIZE; ++i){
    data[i] = rand();
  }
}

int getMax(size_t start, size_t end){
  int max = data[start];
  size_t max_index;
  for(int iter = start+1; iter <= end; iter++){
    if(data[iter] > max){
      max = data[iter];
      max_index = iter;
    }
  }
  return max;
}

void *threadWorker(void *arg){
  int n = (int)arg;

  const size_t start = (DATASIZE / THREADS) * n;
  const size_t end = (start + (DATASIZE / THREADS)) - 1;
  int max = getMax(start, end);
  return (void *)max;
}

int main(void){
  pthread_t threads[THREADS];
  int res[THREADS];
  int i, max;

  srand(time(NULL));
  setRandomData();

  for(int i = 0; i<THREADS; ++i){
    if(0 != pthread_create(&(threads[i]), NULL, threadWorker, (void *)i)){
      perror("pthread_create");
      exit(1);
    }
  }

  for(int i = 0; i<THREADS; ++i){
    if(0 != pthread_join(threads[i], (void **)&(res[i]))){
      perror("pthread_join");
      exit(1);
    }
  }

  max = res[0];
  for(int i = 1; i<THREADS; ++i){
    if(res[i] > max)
      max = res[i];
  }

  printf("max = %d\n", max);

  return 0;
}
