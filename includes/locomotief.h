#include "proces.h"

#ifndef _LOCOMOTIEF_H_
#define _LOCOMOTIEF_H_

#define SNEL      0
#define LANGZAAM  1
#define VOORUIT   2
#define ACHTERUIT 3
#define STOP      4

#define NUM_LOCO_SENS 5

void locomotief_start (struct exec_data *data);

#endif
