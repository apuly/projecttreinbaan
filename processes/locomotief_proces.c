#include <stdlib.h>

#include "../includes/locomotief.h"
#include "../includes/alphabet.h"

#define alpha_c (sizeof(ALPHABET) / sizeof(int))


void locomotief_start(struct exec_data *data)
{

  const int ALPHABET[] = {SNEL, LANGZAAM, STOP, RIJ_VOORUIT, RIJ_ACHTERUIT};
  stuur_alphabet(data, LOCOMOTIEF_PROCES, data->system_id, ALPHABET, alpha_c);
  exit(0);
  while(1)
  {
    
  }
}


