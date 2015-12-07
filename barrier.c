#include "barrier.h"

int 
_SDL_BarrierInit(_SDL_Barrier_t *barrier, int needed)
{
  barrier->needed = needed;
  barrier->called = 0;
  
  barrier->mutex = SDL_CreateMutex();
  barrier->cond  = SDL_CreateCond();
   
  return 0;
}

int 
_SDL_BarrierDestroy(_SDL_Barrier_t *barrier)
{
  SDL_DestroyMutex(barrier->mutex);
  SDL_DestroyCond(barrier->cond);
  
  return 0;
}

int
_SDL_BarrierWait(_SDL_Barrier_t *barrier)
{
  SDL_mutexP(barrier->mutex);
  barrier->called++;
  
  if (barrier->called == barrier->needed) {
    barrier->called = 0;
    SDL_CondBroadcast(barrier->cond);
  } 
  else
    SDL_CondWaitTimeout(barrier->cond, barrier->mutex, 2000);
   
  SDL_mutexV(barrier->mutex);

  return 0;
}

int
_SDL_BarrierDec(_SDL_Barrier_t *barrier)
{
  SDL_mutexP(barrier->mutex);
  barrier->needed--;
  /* SDL_CondBroadcast(barrier->cond); */
  SDL_mutexV(barrier->mutex);

  return 0;
}

int
_SDL_BarrierCheck(_SDL_Barrier_t *barrier)
{
  int retval;

  SDL_mutexP(barrier->mutex);
  retval = barrier->needed;
  SDL_mutexV(barrier->mutex);

  return retval;
}
