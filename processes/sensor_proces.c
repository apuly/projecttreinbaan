#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "../includes/sensor.h"
#include "../includes/alphabet.h"
#include "../includes/proces.h"


#define alpha_c (sizeof(ALPHABET) / sizeof(int))

/*TODO: pas sensor acties aan in de ltsa en alphabet.h*/
const int ALPHABET[] = {HOOG, LAAG};
int alp_size = (sizeof(ALPHABET)/sizeof(int));
int SENSITIVITY[] = {HOOG};

void sensor_start(struct exec_data *data)
{
  char *action;
  
  stuur_alphabet(data, SENSOR_PROCES, data->system_id, ALPHABET, alpha_c);
  exit(0);
  while(1)
  {
    /*Recieve action, change sensitivity, send sensitivity*/
	action = receive_action(action, data);
	change_sens(*action);
	send_sensitivity(SENSITIVITY, data);    
  }
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
	  case HOOG:
	    SENSITIVITY[0] = LAAG;
		break;
	  case LAAG:
	    SENSITIVITY[0] = HOOG;
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



