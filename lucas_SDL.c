#include <stdint.h>

#include "SDL.h"

#include "lucas_SDL.h"
#include "struct.h"
#include "param.h"
#include "barrier.h"

_SDL_Barrier_t *b_draw;
SDL_cond *c_comp;
SDL_mutex *m_comp;

int delay = 100;

Uint32 green; 

/* maybe atomic? */
int isrunning = 0;

void
lucas_SDL(struct params *pm)
{

  int i;

  struct lucas_ca *cas;

  SDL_Thread **engine_threads = NULL;
  SDL_Thread *draw_thread;
    
  SDL_Event *keyevent = malloc(sizeof(SDL_Event));

  /* atexit(SDL_Quit); */

  /* Creating screen */
 
  /* flags = (fullscreen) ? SDL_FULLSCREEN : 0; */

  /* zeroes in SDL_SetVideoMode will set the width and height of
     the current video mode */

  if (!(SDL_SetVideoMode(0, 0, 32, 
			 SDL_HWSURFACE | 
			 SDL_RESIZABLE |
			 SDL_DOUBLEBUF))) {
    fprintf(stderr, "ERROR [SDL]: Initialize screen error: %s\n", SDL_GetError());
    exit(EXIT_FAILURE);
  } 

  b_draw = malloc(sizeof(_SDL_Barrier_t));
  _SDL_BarrierInit(b_draw, pm->nr_cas + 1);
  c_comp = SDL_CreateCond();
  m_comp = SDL_CreateMutex();

  engine_threads = malloc(sizeof(SDL_Thread *) * pm->nr_cas);

  isrunning = 1;

  cas=pm->cas;

  for (i=0; i<pm->nr_cas; i++) {
    engine_threads[i] = SDL_CreateThread(engine, cas);
    cas = cas->next;
  }

  draw_thread = SDL_CreateThread(draw, pm);

  while (isrunning) {
    SDL_WaitEvent(keyevent);

    switch (keyevent->type) {
    case SDL_KEYDOWN:
      if (keyevent->key.keysym.sym == SDLK_q) {
        isrunning = 0;
      }
      else if (keyevent->key.keysym.sym == SDLK_a) {
	if (delay < 500)
	  delay *= 2;
      }
      else if (keyevent->key.keysym.sym == SDLK_s) {
	if (delay > 5)
	  delay /= 2;
      }
      break;
    }
  }

  free(keyevent);


  for (i=0; i<pm->nr_cas; i++) {
    SDL_WaitThread(engine_threads[i], NULL);
  }

  SDL_WaitThread(draw_thread, NULL);

  _SDL_BarrierDestroy(b_draw);
  SDL_DestroyMutex(m_comp);
  SDL_DestroyCond(c_comp);

  free(b_draw);

  SDL_Quit();
}

int 
engine(void *data)
{
  struct lucas_ca *ca = data;

  int i, j; 
  
  uint8_t tmp = 0;

  uint8_t *tmpgrid;

  tmpgrid = calloc(ca->size, sizeof(uint8_t));

  for (i=0; i<ca->moves && isrunning; i++) {
    SDL_mutexP(m_comp);
    SDL_CondWait(c_comp, m_comp);
    SDL_mutexV(m_comp);
    
    tmp = ((ca->grid[ca->size-1])<<2) | ((ca->grid[0])<<1) | ca->grid[1];
    if (ca->rule & (1 << tmp))
      tmpgrid[0] = 1;

    tmp = ((ca->grid[ca->size-2])<<2) | 
      ((ca->grid[ca->size-1])<<1) | 
      ca->grid[0];
    if (ca->rule & (1 << tmp))
      tmpgrid[ca->size-1] = 1;

    for (j=1; j<ca->size-1 && isrunning; j++) {
      tmp = ((ca->grid[j-1])<<2) | ((ca->grid[j])<<1) | ca->grid[j+1];

      if ((tmp >= 0) && (tmp <= 7) && (ca->rule & 1<<(tmp))) {
	tmpgrid[j] = 1;
      }
    }

    memcpy(ca->grid, tmpgrid, ca->size*sizeof(uint8_t));
    bzero(tmpgrid, ca->size*sizeof(uint8_t));

    _SDL_BarrierWait(b_draw);
  }

  _SDL_BarrierDec(b_draw);

  free(tmpgrid);

  return 0;

}

int
draw(void *data)
{
  int i;
  
  int line=0;
  
  SDL_Surface *screen;
  SDL_Surface *subscr[4];
  SDL_Rect subsurf[4];
  
  struct params *pm = data;
  struct lucas_ca *cas;
  Uint8 nr_cas = (Uint8) pm->nr_cas;

  screen = SDL_GetVideoSurface();

  green = SDL_MapRGB(screen->format, 0xff, 0x00, 0xff);

  switch (nr_cas) {
  case 1:
    subsurf[0].x = 0;
    subsurf[0].y = 0;
    subsurf[0].w = screen->w;
    subsurf[0].h = screen->h;
    break;
  case 2:
    subsurf[0].x = 0;
    subsurf[0].y = 0;
    subsurf[0].w = screen->w / 2 - 1;
    subsurf[0].h = screen->h;

    subsurf[1].x = subsurf[0].w + 2;
    subsurf[1].y = 0;
    subsurf[1].w = screen->w / 2 - 1;
    subsurf[1].h = screen->h;
    break;
  case 4:
    subsurf[0].x = 0;
    subsurf[0].y = 0;
    subsurf[0].w = screen->w / 2 - 5;
    subsurf[0].h = screen->h / 2 - 5;

    subsurf[1].x = screen->w / 2 + 5;
    subsurf[1].y = 0;
    subsurf[1].w = screen->w / 2 - 5;
    subsurf[1].h = screen->h / 2 - 5;

    subsurf[2].x = 0;
    subsurf[2].y = screen->h / 2 + 5;
    subsurf[2].w = screen->w / 2 - 5;
    subsurf[2].h = screen->h / 2 - 5;

    subsurf[3].x = screen->w / 2 + 5;
    subsurf[3].y = screen->h / 2 + 5;
    subsurf[3].w = screen->w / 2 - 5;
    subsurf[3].h = screen->h / 2 - 5;
    break;
  }

  for (i=0; i<nr_cas; i++)
    subscr[i] = SDL_CreateRGBSurface(SDL_HWSURFACE, subsurf[i].w, 
				     subsurf[i].h, 32, 0, 0, 0, 0);

  while (isrunning) {
    cas = pm->cas;
    for (i=0; i<nr_cas; i++) {
      draw_line(cas, subscr[i], subsurf[i], line);
      cas = cas->next;
    }

    /* SDL_UpdateRect(screen, 0, 0, screen->w, screen->h);  */
    SDL_Flip(screen);
    line++;

    SDL_Delay(delay);

    SDL_mutexP(m_comp);
    SDL_CondBroadcast(c_comp);
    SDL_mutexV(m_comp);

    if (_SDL_BarrierCheck(b_draw) > 1)
      _SDL_BarrierWait(b_draw);
  }

  isrunning = 0;
    
  return 0;
}

void
draw_line(struct lucas_ca *ca, SDL_Surface *subscr, SDL_Rect subsurf,
	  Uint32 line)
{
  int i;

  Uint16 block;

  SDL_Surface *screen;
  SDL_Surface *line_surface;
  SDL_Rect box;
  SDL_Rect line_rect;
  SDL_Rect scroll, scrool;

  if (line > ca->moves)
    return;

  screen = SDL_GetVideoSurface();

  block = subsurf.w / ca->size;

  box.w = box.h = block;
  box.y = box.x = 0;

  scroll.x = 0;
  scroll.y = block;
  scroll.w = subsurf.w;
  scroll.h = subsurf.h - (subsurf.h % block);
  
  scrool.x = 0;
  scrool.y = 0;
  scrool.w = subsurf.w;
  scrool.h = subsurf.h - block - (subsurf.h % block);

  line_rect.w = subsurf.w;
  line_rect.h = block;
  line_rect.x = 0;
  line_rect.y = (line * block > subsurf.h) ? subsurf.h - block : line*block;

  line_surface = SDL_CreateRGBSurface(SDL_HWSURFACE, subsurf.w, 
				      block, 32, 0, 0, 0, 0);

  for (i=0; i<ca->size && isrunning; i++) {
      if (ca->grid[i] == 1) {
	box.x = i*block;
	SDL_FillRect(line_surface, &box, green);
      }
  }

  if (line * block > (int) subsurf.h)
    SDL_BlitSurface(subscr, &scroll, subscr, &scrool);
  
  SDL_BlitSurface(line_surface, NULL, subscr, &line_rect);
  SDL_BlitSurface(subscr, NULL, screen, &subsurf);
}
