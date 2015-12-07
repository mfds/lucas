#include "SDL.h"

#include "struct.h"
#include "lucas.h"

int 
draw(void *data)
{
  extern int isrunning;

  int i, j, block;
  
  struct lucas_ca *ca = (struct lucas_ca) data;

  SDL_Surface *screen = lsdl->scr;
  SDL_Rect box;

  Uint32 green;

  green = SDL_MapRGB(screen->format, 0x00, 0xff, 0x00);

  block = lsdl->rec.w / size;

  box.w = box.h = block;

  for (i=0; i<n && isrunning; i++) {
    SDL_SemWaitTimeout(lsdl->read_sem, 1000);
    box.y = i*block;
    for (j=0; j<ca->size && isrunning && i<screen->h; j++) {
      if (ca->grid[j] == 1) {
	box.x = j*block;
	SDL_FillRect(screen, &box, green);
      }
    }
    SDL_UpdateRect(screen, 0,0,0,0);
    SDL_SemPost(write);
  }

  isrunning = 0;

  return 0;

}
