#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include "../includes/wissel.h"
#include "../includes/proces.h"
#include "../includes/command.h"

#define alpha_c (sizeof(ALPHABET) / sizeof(int))

int wissel_change_sens(int action, int *new_sens);

/*function for changing proces into wissel proces */
void wissel_start(struct exec_data data)
{
  const int ALPHABET[] = {RECHT, KROM}; /* alphabet of the wissel proces */
  /* sensitivity arrays needed for send_sensitivity */
  int new_sens[alpha_c], curr_sens[alpha_c];
  int new_len; /* current length of the new_sens */
  int curr_len; /* current length of curr_sens */
  int size, cmd, buff[10], action;

  /* send proces alphabet to sync serv */
  send_alphabet(data, WISSEL_PROCES, data.system_id, ALPHABET, alpha_c);
  
  /* set inital sensitivities and send to sync serv */
  curr_sens[0] = KROM;
  curr_len = 1;
  set_sensitivity(data, curr_sens, curr_len);

  while(KILL_PROCES)
  {
    size = receive_action(data, &cmd, buff); /* receive action from sync serv */
    if (cmd == SET_STATE){
      /*if state of proces has changed, lookup new sensitivity and send to
        sync serv */
      action = buff[0];
      new_len = wissel_change_sens(action, new_sens);
      curr_len = send_sensitivity(data, curr_sens, curr_len, new_sens, new_len);
    } else if (cmd == EXIT){
      exit(0);
    }
  }
}

/* lookup new sensitivity based on new state
   takes the new action and pointer to new_sens
   returns length of new_sens
*/
int wissel_change_sens(int action, int *new_sens)
{
  /* lookup new sensitivity and write to new_sens */
  switch(action){
    case RECHT:
      new_sens[0] = KROM;
      return 1;
      break;
    case KROM:
       new_sens[0] = RECHT;
       return 1;
       break; 
    default:
      printf("Command %d id invalid!\n", action);
      return -1;
      break;
  }
}

