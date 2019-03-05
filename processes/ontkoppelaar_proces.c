#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../includes/ontkoppelaar.h"
#include "../includes/proces.h"
#include "../includes/command.h"

#define alpha_c (sizeof(ALPHABET) / sizeof(int))

int ontkoppel_change_sens(int action, int *new_sens);

/*function for changing proces into ontkoppel proces */
void ontkoppel_start(struct exec_data data)
{
  /* alphabet of the ontkoppel proces */
  const int ALPHABET[] = {HOOG_LAAG};

  /* sensitivity arrays for send_sensitivity */
  int new_sens[alpha_c], curr_sens[alpha_c];
  int new_len; /* the length of new_sens */
  int curr_len; /* the length of curr_sens */
  int size, cmd, buff[10], action;

  /* send alphabet to synchronisation server */
  send_alphabet(data, ONTKOPPEL_PROCES, data.system_id, ALPHABET, alpha_c);

  /* initial sensitivity and send to sync serv */
  curr_sens[0] = HOOG_LAAG;
  curr_len = 1;
  set_sensitivity(data, curr_sens, curr_len);

  while(1){
    size = receive_action(data, &cmd, buff); /* receive action from sync serv */
    if (cmd == SET_STATE){ 
      /* if new state of proces, update sensitivity and send to sync serv */
      action = buff[0];
      new_len = ontkoppel_change_sens(action, new_sens);
      curr_len = send_sensitivity(data, curr_sens, curr_len, new_sens, new_len);
    } else if (cmd == EXIT){
     exit(0);
    }
  }
}

/* update sensitivity based on new action
   takes new state and pointer to new_sens, return new size of new_sens */
int ontkoppel_change_sens(int action, int *new_sens)
{
  /*change the new sensitivity*/
  switch(action){
    case HOOG_LAAG:
      new_sens[0] = HOOG_LAAG;
      return 1;
      break;
    default:
      printf("Command %d id invalid!\n", action);
      return -1;
      break;
  }
}

