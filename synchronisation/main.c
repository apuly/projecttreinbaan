#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <poll.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "../includes/locomotief.h"
#include "../includes/ontkoppelaar.h"
#include "../includes/wissel.h"
#include "../includes/sensor.h"
#include "../includes/proces.h"
#include "../includes/sync.h"
#include "../includes/command.h"

#define FINAL_MAIN

_PROTOTYPE( char data_available, (struct proces_data *data));

_PROTOTYPE( void start_procs, (struct proces_data *data, int num_processes, 
                                   char proces));
_PROTOTYPE( void read_command, (int fd, int *command, int *param, int *paramc));

_PROTOTYPE( void handle_set_alpha, (int proces_id, int system_id,
                                      int paramc, int *paramv));

_PROTOTYPE( void handle_set_sens, (int proces_id, int system_id,
                                      int paramc, int *paramv));

_PROTOTYPE( void handle_rem_sens, (int proces_id, int system_id,
                                      int paramc, int *paramv));

struct sensitivity ***glob_sens;
struct proces_data **glob_data;

char data_available(struct proces_data *data)
{
  struct pollfd p;
  p.fd = data->read_fd;
  p.events = POLLIN;
  return poll(&p, 1, 0) == 1;
}

void read_command(int fd, int *command, int *param, int *paramc)
{
  int len, i;
  read(fd, command, sizeof(int));
  read(fd, &len, sizeof(int));
  for (i=0; i<len; i++){
    read(fd, param+i, sizeof(int));
  }
  *paramc = len;
}

void synchronise(struct proces_data ***data_, struct sensitivity ****sens_)
{
  int i, j, num_procs;
  struct proces_data **data;
  struct sensitivity ***sens;
  int command, param[10], paramc;
  
  data = *data_;
  sens = *sens_;
  printf("starting to synchronise!\n");
  while(1)
  {
    for(i=0; i<NUM_PROCES_TYPES; i++){
      num_procs = get_num_procs(i);
      for(j=0; j<num_procs; j++){
        if (data_available(&(data[i][j]))){ 
          printf("received command from %d:%d!\n", i, j);
          read_command(data[i][j].read_fd, &command, param, &paramc);
          
          printf("cmd: %d, paramc: %d.\n", command, paramc);
        }
      }
    }
  }
}

void handle_command(int proces_id, int system_id, 
                      int command, int paramc, int *paramv)
{
  switch (command){
    case SET_ALPHA:
      handle_set_alpha(proces_id, system_id, paramc, paramv);
      break;
    case SET_SENS:
      handle_set_sens(proces_id, system_id, paramc, paramv);
      break;
    case REM_SENS:
      handle_rem_sens(proces_id, system_id, paramc, paramv);
      break;
    default:
      printf("recieved invalid command id: %d\n", command);
      break;
  }
}


void handle_set_sens(int proces_id, int system_id, int paramc, int *paramv)
{
  int i, sens_id;
  struct sensitivity sens;
  for (i=0; i<paramc; i++){
    sens_id = paramv[i];
    sens = glob_sens[proces_id][system_id][sens_id];
    if (++sens.cur == sens.max){
      printf("Sensitivity for %d:%d reached. doing stuff!\n", proces_id,
                                                                  system_id);
    }
  }
  
}


void handle_rem_sens(int proces_id, int system_id, int paramc, int *paramv)
{
  int i, sens_id;
  for (i=0; i<paramc; i++){
    sens_id = paramv[i];
    glob_sens[proces_id][system_id][sens_id].cur--;
  }
}

void handle_set_alpha(int proces_id, int system_id, int paramc, int *paramv)
{
  int i, sens_id;
  for (i=0; i<paramc; i++){
    sens_id = paramv[i];
    glob_sens[proces_id][system_id][sens_id].max++;
 }
}



#ifdef FINAL_MAIN

int main(int argc, char **argv)
{
  initialise(&glob_data, &glob_sens);
  synchronise(&glob_data, &glob_sens);
}
#else
int main(int argc, char **argv)
{
  struct proces_data data;
  int command, param_buff[10], paramc, i;
  init_process(&data, LOCOMOTIEF_PROCES, 0);
  printf("executing test main!\n");
  while(1){
    if (data_available(&data)){
      printf("received data from proces!\n");
      read_command(data.read_fd, &command, param_buff, &paramc);
      printf("received command %d!\n", command);
      printf("%d parameters!\n", paramc);
      for (i=0; i<paramc; i++){
        printf("%d\t", param_buff[i]);
      }
      printf("\n");
    }  
  }
}
#endif
