#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "../includes/locomotief.h"
#include "../includes/proces.h"
#define alpha_c (sizeof(ALPHABET) / sizeof(int))

const int ALPHABET[] = {SNEL, LANGZAAM, STOP, RIJ_VOORUIT, RIJ_ACHTERUIT};
int SENSITIVITY[alpha_c] = {RIJ_VOORUIT};

void locomotief_start(struct exec_data *data)
{
  char *action;

  const int ALPHABET[] = {SNEL, LANGZAAM, STOP, VOORUIT, ACHTERUIT};
  stuur_alphabet(data, ALPHABET, alpha_c);
  while(1){
    sleep(10);
  }
  
  printf("exiting!");
}

char *receive_action(char *action, struct exec_data *data)
{
	read(data->read_fd, &action, sizeof(action));
	return action;
}

void change_sens(int action)
{
  switch(action)
    {
	  case SNEL:
	    SENSITIVITY[0] = LANGZAAM;
		SENSITIVITY[1] = STOP;
		break;
	  case LANGZAAM:
	    SENSITIVITY[0] = SNEL;
		SENSITIVITY[1] = STOP;
		break; 
	  case STOP:
	    SENSITIVITY[0] = RIJ_VOORUIT;
	    SENSITIVITY[0] = RIJ_ACHTERUIT;
		break; 
	  case RIJ_VOORUIT:
	    SENSITIVITY[0] = LANGZAAM;
		SENSITIVITY[1] = SNEL;
		break; 
	  case RIJ_ACHTERUIT:
	    SENSITIVITY[0] = LANGZAAM;
		SENSITIVITY[1] = SNEL;
		break; 
	  default:
        printf("Command %d id invalid!\n", action);
        break;
    }
}

void send_sensitivity(int *sensitivity, struct exec_data *data)
{
	write(data->write_fd, &sensitivity, sizeof(sensitivity));
}



