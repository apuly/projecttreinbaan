#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include "../includes/wissel.h"
#include "../includes/proces.h"


#define alpha_c (sizeof(ALPHABET) / sizeof(int))

void wissel_start(struct exec_data data)
{
  const int ALPHABET[] = {RECHT, KROM};
  stuur_alphabet(&data, ALPHABET, alpha_c);
  while(KILL_PROCES){
    sleep(10);
  }
  printf("exiting");
  exit(0);
}
