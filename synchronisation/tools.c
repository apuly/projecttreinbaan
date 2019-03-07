#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "../includes/locomotief.h"
#include "../includes/ontkoppelaar.h"
#include "../includes/wissel.h"
#include "../includes/sensor.h"
#include "../includes/proces.h"
#include "../includes/sync.h"

int get_num_procs(int proces_type)
{
  switch (proces_type) {
    case LOCOMOTIEF_PROCES:
      return NUM_LOCOMOTIEF;
      break;
    case WISSEL_PROCES:
      return NUM_WISSELS;
      break;
    case ONTKOPPEL_PROCES:
      return NUM_ONTKOPPEL;
      break;
    case SENSOR_PROCES:
      return NUM_SENSORS;
      break;
    case SYSTEM_SENS:
      return NUM_SYSTEM_SENS;
      break;
    default:
      return 0;
      break;
  }
}

int get_num_sens(int proces_type)
{
  switch (proces_type) {
    case LOCOMOTIEF_PROCES:
      return NUM_LOCO_SENS;
      break;
    case WISSEL_PROCES:
      return NUM_WISSEL_SENS;
      break;
    case ONTKOPPEL_PROCES:
      return NUM_ONTKOPPEL_SENS;
      break;
    case SENSOR_PROCES:
      return NUM_SENSOR_SENS;
      break;
    default:
      return 0;
      break;
  }
}

void print_sens(struct sensitivity ****sens)
{
  #define TOOLS_USE_FOR_ 
  int i, j, k, num_proc, num_sens;
  struct sensitivity s;
  #ifdef TOOLS_USE_FOR
    for (i=0; i<NUM_PROCES_TYPES; i++){
  #else
    i = 1;
  #endif
    printf("%d\n", i);
    num_proc = get_num_procs(i);
    for (j=0; j<num_proc; j++){
      printf("\t%d\n", j);
      num_sens = get_num_sens(i);
      for (k=0; k<num_sens; k++){
        s = (*sens)[i][j][k];
        printf("\t\t%d:%d\n", s.cur, s.max);
      }
    }
  #ifdef TOOLS_USE_FOR
  }
  #endif
}

