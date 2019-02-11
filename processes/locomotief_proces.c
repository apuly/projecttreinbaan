#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "../includes/locomotief.h"
#include "../includes/proces.h"
#define alpha_c (sizeof(ALPHABET) / sizeof(int))


void locomotief_start(struct exec_data *data)
{

  const int ALPHABET[] = {SNEL, LANGZAAM, STOP, VOORUIT, ACHTERUIT};
  stuur_alphabet(data, ALPHABET, alpha_c);
  while(1){
    sleep(10);
  }
  
  printf("exiting!");
}


