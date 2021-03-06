#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <errno.h>

#define WIDTH 78
#define HEIGHT 23
#define MAX_FLY 6
static const char *flyMarkList = "o@*+.#";
#define DRAW_CYCLE 50
#define MIN_SPEED 1.0
#define MAX_SPEED 20.0

int stopRequest;
int drawRequest;
pthread_mutex_t drawMutex;
pthread_cond_t drawCond;

void mSleep(int msec){
  struct timespec ts;
  ts.tv_sec = msec / 1000;
  ts.tv_nsec = (msec % 1000) * 1000000;
  nanosleep(&ts, NULL);
}

int pthread_cond_timedwait_msec(pthread_cond_t *cond, pthread_mutex_t *mutex, long msec){
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  ts.tv_sec += msec / 1000;
  ts.tv_nsec += (msec % 1000) * 1000000;
  if(ts.tv_nsec >= 1000000000){
    ts.tv_sec++;
    ts.tv_nsec -= 1000000000;
  }

  return pthread_cond_timedwait(cond, mutex, &ts);
}

double randDouble(double minValue, double maxValue){
  return minValue + (double)rand() / ((double)RAND_MAX + 1) * (maxValue - minValue);
}

void clearScreen(){
  fputs("\033[2J", stdout);
}

void moveCursor(int x, int y){
  printf("\033[%d;%dH", y, x);
}

typedef struct {
  char mark;
  double x, y;
  double angle;
  double speed;
  pthread_mutex_t mutex;
} Fly;

Fly flyList[MAX_FLY];

void FlyInitRandom(Fly *fly, char mark){
  fly->mark = mark;
  fly->x = randDouble(0, (double)(WIDTH - 1));
  fly->y = randDouble(0, (double)(HEIGHT - 1));
  fly->angle = randDouble(0, M_2_PI);
  fly->speed = randDouble(MIN_SPEED, MAX_SPEED);
  pthread_mutex_init(&fly->mutex, NULL);
}

void FlyDestroy(Fly *fly){
  pthread_mutex_destroy(&fly->mutex);
}

void requestDraw(){
  pthread_mutex_lock(&drawMutex);
  drawRequest = 1;
  pthread_cond_signal(&drawCond);
  pthread_mutex_unlock(&drawMutex);
}

void FlyMove(Fly *fly){
  pthread_mutex_lock(&fly->mutex);

  fly->x += cos(fly->angle);
  fly->y += sin(fly->angle);

  if(fly->x < 0){
    fly->x = 0;
    fly->angle = M_PI - (fly->angle);
  }
  else if(fly->x > (WIDTH - 1)){
    fly->x = (WIDTH - 1);
    fly->angle = M_PI - (fly->angle);
  }
  if(fly->y < 0){
    fly->y = 0;
    fly->angle = -(fly->angle);
  }
  else if(fly->y > (HEIGHT - 1)){
    fly->y = (HEIGHT - 1);
    fly->angle = -(fly->angle);
  }

  pthread_mutex_unlock(&fly->mutex);

  requestDraw();
}

int FlyIsAt(Fly *fly, int x, int y){
  int res;
  // 変更中ならば変更されるまで待つ
  pthread_mutex_lock(&fly->mutex);
  res = ((int)(fly->x) == x) && ((int)(fly->y) == y);
  pthread_mutex_unlock(&fly->mutex);
  return res;
}

void *doMove(void *arg){
  Fly *fly = (Fly *)arg;
  while(!stopRequest){
    FlyMove(fly);
    mSleep((int)(1000.0 / fly->speed));
  }
  return NULL;
}

void drawScreen(){
  moveCursor(0, 0);
  for(int y = 0; y < HEIGHT; ++y){
    for(int x = 0; x < WIDTH; ++x){
      char ch = 0;
      for(int i = 0; i < MAX_FLY; ++i){
        if(FlyIsAt(&flyList[i], x, y)){
          ch = flyList[i].mark;
          break;
        }
      }
      if(ch != 0)
        putchar(ch);
      else if ((y == 0) || (y == (HEIGHT - 1)))
        putchar('-');
      else if ((x == 0) || (x == (WIDTH - 1)))
        putchar('|');
      else
        putchar(' ');
    }
    putchar('\n');
  }
}

void *doDraw(void *arg){
  pthread_mutex_lock(&drawMutex);
  
  while(!stopRequest){
    int err = pthread_cond_timedwait_msec(&drawCond, &drawMutex, 1000);
    if(err != 0 && err != ETIMEDOUT){
      perror("pthread_cond_timedwait");
      exit(1);
    }
    while(drawRequest && !stopRequest){
      drawRequest = 0;
      pthread_mutex_unlock(&drawMutex);
      drawScreen();
      pthread_mutex_lock(&drawMutex);
    }
  }

  pthread_mutex_unlock(&drawMutex);
  return NULL;
}

int main(){
  pthread_t drawThread;
  pthread_t moveThread[MAX_FLY];
  char buf[40];

  srand((unsigned int)time(NULL));
  drawRequest = 0;
  pthread_mutex_init(&drawMutex, NULL);
  pthread_cond_init(&drawCond, NULL);
  clearScreen();
  for(int i = 0; i < MAX_FLY; ++i){
    FlyInitRandom(&flyList[i], flyMarkList[i]);
  }

  for(int i = 0; i < MAX_FLY; ++i){
    if(0 != pthread_create(&moveThread[i], NULL, doMove, (void *)&flyList[i])){
      perror("pthread_create");
      exit(1);
    }
  }
  pthread_create(&drawThread, NULL, doDraw, NULL);
  
  fgets(buf, sizeof(buf), stdin);
  stopRequest = 1;
  pthread_join(drawThread, NULL);
  for(int i = 0; i < MAX_FLY; ++i){
    pthread_join(moveThread[i], NULL);
    FlyDestroy(&flyList[i]);
  }

  pthread_mutex_destroy(&drawMutex);
  pthread_cond_destroy(&drawCond);
  
  return 0;
}
