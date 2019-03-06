#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "../includes/locomotief.h"
#include "../includes/proces.h"
#include "../includes/command.h"
#include "../includes/debug.h"


#define alpha_c (sizeof(ALPHABET) / sizeof(int))

int loco_change_sens(int action, int *new_sens);

/* function for making a proces a locomotive proces */
void locomotief_start(struct exec_data data)
{
  /* the alphabet of the proces */
  const int ALPHABET[] = {SNEL, LANGZAAM, STOP, VOORUIT, ACHTERUIT};

  /* empty array of new and current sensitivites, needed for send_sensitivity */
  int new_sens[alpha_c], curr_sens[alpha_c];
  int new_len; /* length of new_sens */
  int curr_len; /* length of curr_sens */
  int buff[10], cmd, size, action;

  /*send the alphabet to the synchronisation server */
  send_alphabet(data, LOCOMOTIEF_PROCES, data.system_id, ALPHABET, alpha_c);

  /* send initial sensitivities to synchronisation server */
  curr_sens[0] = VOORUIT;
  curr_sens[1] = ACHTERUIT;
  curr_len = 2;
  set_sensitivity(data, curr_sens, curr_len);

  while(KILL_PROCES)
  {
    size = receive_action(data, &cmd, buff); /* receive command from sync serv*/
    #if DEBUG_LOCO
    printf("cmd: %d, size: %d\n");
    #endif
    if (cmd == SET_STATE){
     /*of state of proces has changed, build new sensitivity list and send to 
       sync serv */
      #if DEBUG_LOCO
      printf("received set_state command");
      #endif
      action = buff[0];
      new_len = loco_change_sens(action, new_sens); 
      curr_len = send_sensitivity(data, curr_sens, curr_len, new_sens, new_len);
    } else if (cmd == EXIT) {
      exit(0);
    }
  }
}

int loco_change_sens(int action, int *new_sens)
{
  /* change sensitivity based on new state */
  switch(action){
    case SNEL:
      new_sens[0] = LANGZAAM;
      new_sens[1] = STOP;
      return 2;
      break;
    case LANGZAAM:
      new_sens[0] = SNEL;
      new_sens[1] = STOP;
      return 2;
      break; 
    case STOP:
      new_sens[0] = VOORUIT;
      new_sens[1] = ACHTERUIT;
      new_sens[2] = SNEL;
      new_sens[3] = LANGZAAM;
      return 4;
      break; 
    case VOORUIT:
      new_sens[0] = LANGZAAM;
      new_sens[1] = SNEL;
      new_sens[2] = STOP;
      return 3;
      break; 
    case ACHTERUIT:
      new_sens[0] = LANGZAAM;
      new_sens[1] = SNEL;
      new_sens[2] = STOP;
      return 3;
      break; 
    default:
      printf("Command %d id invalid!\n", action);
      return -1;
      break;
  }
}

