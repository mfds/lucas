#include <stdio.h>
#include <stdlib.h>

#include "lucas_SDL.h"

#include "param.h"

int main(int argc, char **argv) 
{
  struct params *pm = NULL;

  if (argc == 2)
    pm = init_params(argv[1]);

  if (pm) {
    pm->cas = reverse_pointers(pm->cas);
    lucas_SDL(pm);
  }
  
  return 0;

}
