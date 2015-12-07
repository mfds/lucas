#include "SDL.h"

#include "param.h"

void
lucas_SDL(struct params *pm);

int 
engine(void *data);

int
draw(void *data);

void
draw_line(struct lucas_ca *ca, SDL_Surface *subscr, SDL_Rect subsurf,
	  Uint32 line);

