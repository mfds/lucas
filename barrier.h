#include "SDL.h"

typedef struct {
  int needed;
  int called;
  SDL_mutex *mutex;
  SDL_cond *cond;
} _SDL_Barrier_t;

int 
_SDL_BarrierInit(_SDL_Barrier_t *barrier, int needed);

int 
_SDL_BarrierDestroy(_SDL_Barrier_t *barrier);

int 
_SDL_BarrierWait(_SDL_Barrier_t *barrier);

int
_SDL_BarrierDec(_SDL_Barrier_t *barrier);

int
_SDL_BarrierCheck(_SDL_Barrier_t *barrier);
