#include <stdlib.h>

#include "../includes/wissel.h"
#include "../includes/alphabet.h"
#include "../includes/proces.h"


#define alpha_c (sizeof(ALPHABET) / sizeof(int))

void wissel_start(struct exec_data *data)
{
  const int ALPHABET[] = {RECHT, KROM};

  stuur_alphabet(data, WISSEL_PROCES, data->system_id, ALPHABET, alpha_c);
  exit(0);
  while(1)
  {
    
  }
}


