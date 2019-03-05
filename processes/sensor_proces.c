#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../includes/sensor.h"
#include "../includes/proces.h"
#include "../includes/command.h"

#define alpha_c (sizeof(ALPHABET) / sizeof(int))

int sensor_change_sens(int action, int *new_sens);
/* function to change proces into sensor proces */
void sensor_start(struct exec_data data)
{
  /* alphabet of sensor proces */
  const int ALPHABET[] = {HOOG, LAAG};

  /* sensitivity arrays, needed for send_sensitivity */
  int new_sens[alpha_c], curr_sens[alpha_c];
  int new_len; /*length of new_sens*/
  int curr_len; /*length of curr_sens */
  int size, cmd, buff[10], action;

  /* send alphabet to sync serv */
  send_alphabet(data, SENSOR_PROCES, data.system_id, ALPHABET, alpha_c);

  /*set initial sensitivity and send to sync serv */
  curr_sens[0] = HOOG;
  curr_len = 1;
  set_sensitivity(data, curr_sens, curr_len);

  while(1){
    size = receive_action(data, &cmd, buff); /*receive action from sync serv */
    if (cmd == SET_STATE){
      /* if new state of proces, lookup new sensitivity and send to sync serv*/
      action = buff[0];
      new_len = sensor_change_sens(action, new_sens);
      curr_len = send_sensitivity(data, curr_sens, curr_len, new_sens, new_len);
    } else if (cmd == EXIT) {
      exit(0);
    }
  }
}

/* update sensitivity based on new state
   takes new state and pointer to new_sens
   returns new size of new_sens */
int sensor_change_sens(int action, int *new_sens)
{
  /*change the new sensitivity*/
  switch(action){
    case HOOG:
      new_sens[0] = LAAG;
      return 1;
      break;
    case LAAG:
      new_sens[0] = HOOG;
      return 1;
      break; 
    default:
      printf("Command %d id invalid!\n", action);
      return -1;
      break;
  }
}

