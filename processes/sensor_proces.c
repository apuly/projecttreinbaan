#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../includes/sensor.h"
#include "../includes/alphabet.h"
#include "../includes/proces.h"


#define alpha_c (sizeof(ALPHABET) / sizeof(int))


const int ALPHABET[] = {HOOG_LAAG};

void sensor_start(struct exec_data *data)
{
  stuur_alphabet(data, ALPHABET, alpha_c);
  while(1)
  {
    sleep(1);    
  }
  printf("exiting!");
}


