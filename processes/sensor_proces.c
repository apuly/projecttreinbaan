#include <stdlib.h>

#include "../includes/sensor.h"
#include "../includes/alphabet.h"
#include "../includes/proces.h"


#define alpha_c (sizeof(ALPHABET) / sizeof(int))


const int ALPHABET[] = {HOOG_LAAG};

void sensor_start(struct exec_data *data)
{
  stuur_alphabet(data, SENSOR_PROCES, data->system_id, ALPHABET, alpha_c);
  exit(0);
  while(1)
  {
    
  }
}


