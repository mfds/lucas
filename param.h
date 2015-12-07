#include <stdint.h>

#include "struct.h"

#ifndef _PARAM_H
#define _PARAM_H

struct params {
  uint8_t nr_cas;
  uint8_t output;
  struct lucas_ca *cas;
};

#endif /* _PARAM_H */

struct params 
*init_params(char *xml_file);

void
destroy_params(struct params *pm);

