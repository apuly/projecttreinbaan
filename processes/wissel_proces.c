#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include "../includes/wissel.h"
#include "../includes/proces.h"

#define alpha_c (sizeof(ALPHABET) / sizeof(int))

/*A wissel process starts at state RECHT, so the sensitivity starts at state KROM*/
const int ALPHABET[] = {RECHT, KROM};
int alp_size = (sizeof(ALPHABET)/sizeof(int));
int SENSITIVITY[] = {KROM};
  
void wissel_start(struct exec_data *data)
{
  char *action;

<<<<<<< HEAD
  stuur_alphabet(data, WISSEL_PROCES, data->system_id, ALPHABET, alpha_c);
  exit(0);
  
  while(1)
  {
    /*Recieve action, change sensitivity, send sensitivity*/
	action = receive_action(action, data);
	change_sens(*action);
	send_sensitivity(SENSITIVITY, data);
=======
  stuur_alphabet(data, ALPHABET, alpha_c);
  while(1){
    sleep(10);
>>>>>>> 0e790f8d6c3836c5251b364c90489ae6564dcb10
  }
  printf("exiting");
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
	  case RECHT:
	    SENSITIVITY[0] = KROM;
		break;
	  case KROM:
	    SENSITIVITY[0] = RECHT;
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


