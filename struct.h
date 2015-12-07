#include <stdint.h>

#ifndef _LUCAS_STRUCT_H
#define _LUCAS_STRUCT_H

#define PROG_NAME "lucas\0"

enum {
  NONE,
  ONE_CENTRAL,
  RANDOM,
  ALTERNATE,
  ALL,
};

struct lucas_ca {
  uint8_t rule;
  uint16_t size;
  uint32_t moves;

  uint16_t width; 
  uint16_t height;

  uint8_t *grid;

  struct lucas_ca *next;
};

#endif /* _LUCAS_STRUCT_H */

struct lucas_ca
*init_ca (uint8_t rule, uint16_t size, uint32_t move, uint8_t pattern);

struct lucas_ca
*concat_ca (struct lucas_ca *ca1, struct lucas_ca *ca2);

struct lucas_ca
*reverse_pointers (struct lucas_ca *cas);

void
destroy_ca(struct lucas_ca *ca);


/*
int 
run_engine(void *ca);
*/
