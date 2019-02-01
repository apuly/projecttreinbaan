#include <stdlib.h>

#include "../includes/ontkoppelaar.h"
#include "../includes/alphabet.h"
#include "../includes/proces.h"


#define alpha_c (sizeof(ALPHABET) / sizeof(int))


void ontkoppel_start(struct exec_data *data)
{
  
  const int ALPHABET[] = {HOOG_LAAG};

  stuur_alphabet(data, ONTKOPPEL_PROCES, data->system_id, ALPHABET, alpha_c);
  exit(0);
  while(1)
  {
    
  }
}


