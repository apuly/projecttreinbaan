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
    default:
      return 0;
      break;
  }
}
