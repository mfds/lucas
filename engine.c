#include <stdlib.h>

#include "SDL.h"

#include "struct.h"
#include "lucas.h"

int 
engine(void *data)
{
  struct lucas_ca *ca = (struct lucas_ca) data;
  struct lucas_SDL *lsdl = (struct lucas_SDL) ca->data;

  int i, j; 
  
  uint8_t tmp = 0;

  uint8_t *tmpgrid;

  tmpgrid = calloc(ca->size, sizeof(uint8_t));

  for (i=0; i<n && isrunning; i++) {
    SDL_SemWait(lsdl->write);
    
    tmp = ((ca->grid[ca->size-1])<<2) | ((ca->grid[0])<<1) | ca->grid[1];
    if (ca->rule & (1 << tmp))
      tmpgrid[0] = 1;

    tmp = ((ca->grid[ca->size-2])<<2) | 
      ((ca->grid[ca->size-1])<<1) | 
      ca->grid[0];
    if (ca->rule & (1 << tmp))
      tmpgrid[ca->size-1] = 1;

    for (j=1; j<ca->size-1 && isrunning; j++) {
      tempno = ((ca->grid[j-1])<<2) | ((ca->grid[j])<<1) | ca->grid[j+1];

      if ((tmp >= 0) && (tmp <= 7) && (ca->rule & 1<<(tmp))) {
	tmpgrid[j] = 1;
      }
    }

    memcpy(ca->grid, tmpgrid, ca->size*sizeof(uint8_t));
    bzero(tmpgrid, ca->size*sizeof(uint8_t));

    SDL_SemPost(read);
  }

  free(tmpgrid);

  isrunning = 0;

  return 0;

}

