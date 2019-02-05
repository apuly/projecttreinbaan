#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "../includes/locomotief.h"
#include "../includes/alphabet.h"
#include "../includes/proces.h"
#define alpha_c (sizeof(ALPHABET) / sizeof(int))


void locomotief_start(struct exec_data *data)
{

  const int ALPHABET[] = {SNEL, LANGZAAM, STOP, RIJ_VOORUIT, RIJ_ACHTERUIT};
  printf("Locomotief %d sending sensitivity!\n", data->system_id);
  
  stuur_alphabet(data, ALPHABET, alpha_c);
  while(1)
  {
    sleep(1);    
  }
  printf("exiting!");
}


