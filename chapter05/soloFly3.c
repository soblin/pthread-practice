#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <errno.h>

#define WIDTH 78
#define HEIGHT 23
#define MAX_FLY 1
#define DRAW_CYCLE 50

int stopRequest;
// pthread_mutex_t print_mutex;
int offset = 1;

void mSleep(int msec){
  struct timespec ts;
  ts.tv_sec = msec / 1000;
  ts.tv_nsec = (msec % 1000) * 1000000;
  nanosleep(&ts, NULL);
}

void clearScreen(){
  fputs("\033[2J", stdout);
}

void moveCursor(int x, int y){
  printf("\033[%d;%dH", y, x);
}

void saveCursor(){
  printf("\0337");
}

void restoreCursor(){
  printf("\0338");
}

typedef struct {
  char mark;
  double x, y;
  double angle;
  double speed;
  double destX, destY;
  int busy;
  pthread_mutex_t mutex;
  pthread_cond_t cond;
} Fly;

Fly flyList[MAX_FLY];

void FlyInitCenter(Fly *fly, char mark){
  fly->mark = mark;
  pthread_mutex_init(&fly->mutex, NULL);
  pthread_cond_init(&fly->cond, NULL);
  fly->x = (double)(WIDTH / 2.0);
  fly->y = (double)(HEIGHT / 2.0);
  fly->angle = 0;
  fly->speed = 2;
  fly->destX = fly->x;
  fly->destY = fly->y;
  fly->busy = 0;
}

void FlyDestroy(Fly *fly){
  pthread_mutex_destroy(&fly->mutex);
  pthread_cond_destroy(&fly->cond);
}

void FlyMove(Fly *fly){
  pthread_mutex_lock(&fly->mutex);
  fly->x += cos(fly->angle);
  fly->y += sin(fly->angle);
  pthread_mutex_unlock(&fly->mutex);
}

int FlyIsAt(Fly *fly, int x, int y){
  pthread_mutex_lock(&fly->mutex);
  int res = ((int)(fly->x) == x) && ((int)(fly->y) == y);
  pthread_mutex_unlock(&fly->mutex);
  return res;
}

void FlySetDirection(Fly *fly){
  pthread_mutex_lock(&fly->mutex);
  double dx = (fly->destX) - (fly->x);
  double dy = (fly->destY) - (fly->y);
  fly->angle = atan2(dy, dx);
  fly->speed = hypot(dx, dy) / 5.0;
  if(fly->speed < 2)
    fly->speed = 2;
  pthread_mutex_unlock(&fly->mutex);
}

double FlyDistanceToDestination(Fly *fly){
  pthread_mutex_lock(&fly->mutex);
  double dx = (fly->destX) - (fly->x);
  double dy = (fly->destY) - (fly->y);
  double res = hypot(dx, dy);
  pthread_mutex_unlock(&fly->mutex);
  return res;
}

int FlySetDestination(Fly *fly, double x, double y){
  if(fly->busy)
    return 0;

  pthread_mutex_lock(&fly->mutex);
  fly->destX = x;
  fly->destY = y;
  offset += 1;
  pthread_cond_signal(&fly->cond);
  pthread_mutex_unlock(&fly->mutex);
  return 1;
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

int FlyWaitForSetDestination(Fly *fly, long msec){
  int res;
  pthread_mutex_lock(&fly->mutex);
  switch(pthread_cond_timedwait_msec(&fly->cond, &fly->mutex, msec)){
    case 0:
      res = 1;
      break;
    case ETIMEDOUT:
      res = 0;
      break;
    default:
      perror("pthread_cond_timedwait");
      exit(1);
  }
  pthread_mutex_unlock(&fly->mutex);
  return res;
}

void *doMove(void *arg){
  Fly *fly = (Fly *)arg;
  while(!stopRequest){
    fly->busy = 0;

    if(!FlyWaitForSetDestination(fly, 100))
      continue;
    
    if(FlyDistanceToDestination(fly) < 1)
      continue;
    
    fly->busy = 1;
    FlySetDirection(fly);
    while((FlyDistanceToDestination(fly) >= 1) && !stopRequest){
      FlyMove(fly);
      mSleep((int)(1000.0 / fly->speed));
    }
  }

  return NULL;
}

void drawScreen(){
  //  pthread_mutex_lock(&print_mutex);
  saveCursor();
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
      else if((y == 0) || (y == HEIGHT-1))
        putchar('-');
      else if((x == 0) || (x == WIDTH-1))
        putchar('|');
      else
        putchar(' ');
    }
    putchar('\n');
  }

  restoreCursor();
  fflush(stdout);
  //  pthread_mutex_unlock(&print_mutex);
}

void *doDraw(void *arg){
  while(!stopRequest){
    drawScreen();
    mSleep(DRAW_CYCLE);
  }

  return NULL;
}

int main(){
  pthread_t drawThread;
  pthread_t moveThread;
  char buf[40], *cp;
  double destX, destY;

  clearScreen();
  FlyInitCenter(&flyList[0], '@');

  pthread_create(&moveThread, NULL, doMove, (void *)&flyList[0]);
  pthread_create(&drawThread, NULL, doDraw, NULL);
      
  fflush(stdout);
  while(1){
    //    pthread_mutex_lock(&print_mutex);
    moveCursor(0, HEIGHT+offset);
    printf("Destination? :");
    fflush(stdout);
    //    pthread_mutex_unlock(&print_mutex);
    fgets(buf, sizeof(buf), stdin);
    if(strncmp(buf, "stop", 4) == 0)
      break;
    destX = strtod(buf, &cp);
    destY = strtod(cp, &cp);
    if(!FlySetDestination(&flyList[0], destX, destY)){
      //      pthread_mutex_lock(&print_mutex);
      printf("The fly is busy now. Try later.\n");
      offset += 2;
      //      pthread_mutex_unlock(&print_mutex);
    }
  }

  stopRequest = 1;

  pthread_join(drawThread, NULL);
  pthread_join(moveThread, NULL);
  FlyDestroy(&flyList[0]);

  return 0;
}
