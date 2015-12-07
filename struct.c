#include <stdint.h>
#include <stdlib.h>

#include <time.h>

#include "struct.h"

struct lucas_ca 
*init_ca(uint8_t rule, uint16_t size, uint32_t moves, uint8_t pattern)
{
  int i;
  struct lucas_ca *ca;

  ca = malloc(sizeof(struct lucas_ca));

  ca->rule = rule;
  ca->size = size;
  ca->moves = moves;

  ca->width = 0;
  ca->height = 0;

  ca->grid = calloc(ca->size, sizeof(uint8_t)); 

  switch (pattern) {
  case ONE_CENTRAL:
    ca->grid[ca->size/2] = 1;
    break;
  case RANDOM:
    srand(time(NULL));
    for (i=0; i<ca->size; i++)
      ca->grid[i] = rand()%2;
    break;
  case ALTERNATE:
    for (i=0; i<ca->size; i++)
      ca->grid[i] = i%2;
    break;
  case ALL:
    for (i=0; i<ca->size; i++)
      ca->grid[i] = 1;
    break;
  case NONE:
    break;
  }

  ca->next = NULL;

  return ca;
}

struct lucas_ca
*concat_ca (struct lucas_ca *ca1, struct lucas_ca *ca2)
{
  struct lucas_ca *tmp;

  tmp = ca1;

  while (tmp->next != NULL)
    tmp = tmp->next;

  tmp->next = ca2;

  return ca1;
}

struct lucas_ca
*reverse_pointers (struct lucas_ca *cas)
{
  struct lucas_ca *prev = NULL;
  struct lucas_ca *next = NULL;

  if (cas && cas->next) {
    while (cas->next) {
      next = cas->next;
      cas->next = prev;
      prev = cas;
      cas = next;
    }
    cas->next = prev;
  }

  return cas;
}

void
destroy_ca(struct lucas_ca *cas)
{
  struct lucas_ca *tmp;

  while (cas != NULL) {
    tmp = cas;
    cas = cas->next;
    free(tmp->grid);
    free(tmp);
  }
}

