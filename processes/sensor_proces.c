#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../includes/sensor.h"
#include "../includes/proces.h"


#define alpha_c (sizeof(ALPHABET) / sizeof(int))


const int ALPHABET[] = {HOOG, LAAG};

void sensor_start(struct exec_data *data)
{
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



