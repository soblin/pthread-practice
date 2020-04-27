#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define DATASIZE (100000)
#define THREADS 10

int data[DATASIZE];

typedef struct {
  size_t start;
  size_t end;
} ThreadCtx;

void setRandomData(){
  for(int i = 0; i<DATASIZE; ++i){
    data[i] = rand();
  }
}

int getMax(ThreadCtx *arg){
  size_t start = arg->start;
  size_t end = arg->end;
  size_t index;
  int max = data[start];
  for(index = start+1; index <= end; ++index){
    if(data[index] > max)
      max = data[index];
  }

  return max;
}

void *threadWorker(void *arg){
  int n = (int)arg;
  ThreadCtx ctx;
  ctx.start = (DATASIZE / THREADS) * n;
  ctx.end = ctx.start + (DATASIZE / THREADS) - 1;
  int max = getMax(&ctx);
  return (void *)max;
}

int main(void){
  pthread_t threads[THREADS];
  int res[THREADS];

  srand(time(NULL));
  setRandomData();

  for(int i = 0; i<THREADS; ++i){
    if(0 != pthread_create(&(threads[i]), NULL, threadWorker, (void *)i)){
      perror("pthread_create");
      exit(1);
    }
  }

  int max = res[0];
  for(int i = 1; i<THREADS; ++i){
    if(max < res[i])
      max = res[i];
  }

  printf("max = %d\n", max);
}
