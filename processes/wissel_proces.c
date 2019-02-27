#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include "../includes/wissel.h"
#include "../includes/proces.h"

#define alpha_c (sizeof(ALPHABET) / sizeof(int))

/*A wissel process starts at state RECHT, so the sensitivity starts at state KROM*/
const int ALPHABET[] = {RECHT, KROM};
int NEW_SENS[alpha_c];
int CURRENT_SENS[alpha_c];
  
void wissel_start(struct exec_data *data)
{
  char *action;

  send_alphabet(data, WISSEL_PROCES, data->system_id, ALPHABET, alpha_c);
  NEW_SENS[0] = KROM;
  send_sensitivity(CURRENT_SENS, NEW_SENS, data);
  /*exit(0);*/
  
  while(1)
  {
    /*Recieve action, change sensitivity, send old and new sensitivity*/
	action = receive_action(action, data);
	change_sens(*action, NEW_SENS);
	send_sensitivity(CURRENT_SENS, NEW_SENS, data);
  }
}

char *receive_action(char *action, struct exec_data *data)
{
	/*read data from SynchronisationServer pipe*/
	read(data->read_fd, &action, sizeof(action));
	return action;
}

void change_sens(int action, int *NEW_SENS)
{
  /*change the new sensitivity*/
  switch(action)
    {
	  case RECHT:
	    NEW_SENS[0] = KROM;
		break;
	  case KROM:
	    NEW_SENS[0] = RECHT;
		break; 
	  default:
        printf("Command %d id invalid!\n", action);
        break;
    }
}

void send_sensitivity(int *CURRENT_SENS, int *NEW_SENS, struct exec_data *data)
{
	/*write data to SynchronisationServer pipe*/
	write(data->write_fd, &CURRENT_SENS, alpha_c);
	write(data->write_fd, &NEW_SENS, alpha_c);
}


