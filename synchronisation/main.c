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

_PROTOTYPE( void handle_command, (int proces_id, int system_id, 
                      int command, int paramc, int *paramv));

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
  int avail;  
  data = *data_;
  sens = *sens_;
  printf("starting to synchronise!\n");
  while(1)
  {
    for(i=0; i<NUM_PROCES_TYPES; i++){
      num_procs = get_num_procs(i);
      for(j=0; j<num_procs; j++){
        avail = data_available(&(data[i][j]));
        if (avail){

          read_command(data[i][j].read_fd, &command, param, &paramc);
          handle_command(i, j, command, paramc, param);
          /*print_sens(&glob_sens);*/
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
      printf("%d:%d -SET_ALPHA\n", proces_id, system_id);
      handle_set_alpha(proces_id, system_id, paramc, paramv);
      break;
    case SET_SENS:
      printf("%d:%d -SET_SENS\n", proces_id, system_id);
      handle_set_sens(proces_id, system_id, paramc, paramv);
      break;
    case REM_SENS:
      printf("%d:%d -REM_SENS\n", proces_id, system_id);
      handle_rem_sens(proces_id, system_id, paramc, paramv);
      break;
    default:
      printf("recieved invalid command id: %d\n", command);
      break;
  }
}


void handle_set_sens(int proces_id, int system_id, int paramc, int *paramv)
{
  struct sensitivity sens;
  int proc_id, sys_id, sens_id;
  proc_id = paramv[0];
  sys_id = paramv[1];
  sens_id = paramv[2];
  sens = glob_sens[proc_id][sys_id][sens_id];
  sens.cur++;
  if (sens.cur == sens.max){
    /* send action to hardware specific software */
  }
}  


void handle_rem_sens(int proces_id, int system_id, int paramc, int *paramv)
{
  struct sensitivity sens;
  int proc_id, sys_id, sens_id;
  proc_id = paramv[0];
  sys_id = paramv[1];
  sens_id = paramv[2];
  sens = glob_sens[proc_id][sys_id][sens_id];
  sens.cur--;

}

void handle_set_alpha(int proces_id, int system_id, int paramc, int *paramv)
{
  int i, proc_id, sys_id, sens_id;
  proc_id = paramv[0];
  sys_id = paramv[1];
  sens_id = paramv[2];
  glob_sens[proc_id][sys_id][sens_id].max++;
 
}



#ifdef FINAL_MAIN

int main(int argc, char **argv)
{
  initialise(&glob_data, &glob_sens); 
  printf("glob_sens: ");
  printf("%x\n", glob_sens);
  synchronise(&glob_data, &glob_sens);
}
#else
int main(int argc, char **argv)
{
  initialise(&glob_data, &glob_sens);
  printf("glob_sens: %x\n", glob_sens);
}
#endif
