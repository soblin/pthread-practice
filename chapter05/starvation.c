#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define N_RAT (3)

int foodCount;

pthread_mutex_t foodMutex;
pthread_cond_t foodCond;

int stopRequest;

void mSleep(int msec){
  struct timespec ts;
  ts.tv_sec = msec / 1000;
  ts.tv_nsec = (msec % 1000) * 1000000;
  nanosleep(&ts, NULL);
};

void *ratLife(void *arg){
  int n = (int)arg;
  int eatCount = 0;

  while(!stopRequest){
    pthread_mutex_lock(&foodMutex);
    if(pthread_cond_wait(&foodCond, &foodMutex) != 0){
      perror("pthread_cond_wait");
      exit(1);
    }
    printf("Rat-%d: Got food event\n", n);
    if(foodCount > 0){
      printf("Rat-%d: Eat food\n", n);
      foodCount--;
      pthread_mutex_unlock(&foodMutex);
      mSleep(20);
      eatCount++;
    }
    else{
      printf("Rat-%d: orz...\n", n);
      pthread_mutex_unlock(&foodMutex);
      mSleep(50);
    }
  }
  printf("Thread Finished. Rat-%d ate %d times.\n", n, eatCount);

  return NULL;
}

int main(){
  pthread_t ratThreads[N_RAT];
  char buf[40];

  pthread_mutex_init(&foodMutex, NULL);
  pthread_cond_init(&foodCond, NULL);
  foodCount = 0;
  stopRequest = 0;

  for(int i = 0; i < N_RAT; ++i){
    pthread_create(&ratThreads[i], NULL, ratLife, (void *)(i+1));
  }

  while(1){
    fgets(buf, sizeof(buf), stdin);
    if(strncmp(buf, "stop", 4) == 0)
      break;
    int i = atoi(buf);
    if(i > 0){
      pthread_mutex_lock(&foodMutex);
      foodCount += i;
      pthread_cond_broadcast(&foodCond);
      pthread_mutex_unlock(&foodMutex);
    }
  }

  stopRequest = 1;
  // stopRequestが変更されたのを通知するため
  pthread_cond_broadcast(&foodCond);
  for(int i = 0; i < N_RAT; ++i){
    pthread_join(ratThreads[i], NULL);
  }

  pthread_mutex_destroy(&foodMutex);
  pthread_cond_destroy(&foodCond);

  return 0;
}
