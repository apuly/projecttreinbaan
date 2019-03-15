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
#include "../includes/debug.h"

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

struct sensitivity ***glob_sens; /* list of current proces sensitivities */
struct proces_data **glob_data; /* list of proces data needed for execution*/
/* polls proces to see if data has been written to its pipe */
char data_available(struct proces_data *data)
{
  struct pollfd p;
  p.fd = data->read_fd;
  p.events = POLLIN;
  return poll(&p, 1, 0) == 1;
}

void write_command(int fd, int cmd, int *param, int paramc)
{
  int i;
  write(fd, &cmd, sizeof(int));
  write(fd, &paramc, sizeof(int));
  write(fd, param, paramc*sizeof(int));
}

void send_state(struct proces_data data, int proc_id, int sys_id, int state)
{
  int buff_[4], s_ = state;
  write_command(data.write_fd, SET_STATE, &s_, 1);
  buff_[0] = proc_id;
  buff_[1] = sys_id;
  buff_[2] = state;
  write_command(glob_data[HDS_TREIN][0].write_fd, HDS_ACTION, buff_, 3);
}
/*reads a commando from a proces pipe*/
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

/* start the synchronisation server */
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
        /*printf("%d:%d\n", i, j);*/
        avail = data_available(&(data[i][j]));
        if (avail){
          read_command(data[i][j].read_fd, &command, param, &paramc);
          handle_command(i, j, command, paramc, param);
          /*print_sens(&glob_sens);*/
          /*printf("cmd: %d, paramc: %d.\n", command, paramc);*/
        }
      }
    }
  }
}

/* handle an incoming command */
void handle_command(int proces_id, int system_id, 
                      int command, int paramc, int *paramv)
{
  switch (command){
    case SET_ALPHA:
      #if DEBUG_SYNC
      printf("%d:%d -SET_ALPHA\n", proces_id, system_id);
      #endif
      handle_set_alpha(proces_id, system_id, paramc, paramv);
      break;
    case SET_SENS:
      #if DEBUG_SYNC
      printf("%d:%d -SET_SENS\n", proces_id, system_id);
      #endif
      handle_set_sens(proces_id, system_id, paramc, paramv);
      break;
    case REM_SENS:
      #if DEBUG_SYNC
      printf("%d:%d -REM_SENS\n", proces_id, system_id);
      #endif
      handle_rem_sens(proces_id, system_id, paramc, paramv);
      break;
    default:
      printf("recieved invalid command id: %d\n", command);
      break;
  }
}

/* handle the SET_SENS command, add a sensitivity to the sync server */
void handle_set_sens(proces_id, system_id, paramc, paramv)
int proces_id; /* local proces_id of the source of the command */
int system_id; /* system_id of the source of the command */
int paramc; /* the number of parameters in the command */
int *paramv; /* pointer to the array of parameters */
{
  struct sensitivity *sens;
  int proc_id, sys_id, sens_id;
  struct proces_data data;
  proc_id = paramv[0];
  sys_id = paramv[1];
  sens_id = paramv[2];
  sens = &(glob_sens[proc_id][sys_id][sens_id]);
  sens->cur++;
  if (sens->cur == sens->max){
    data = glob_data[proc_id][sys_id];
    send_state(data, proc_id, sys_id, sens_id);
    /* send action to hardware specific software */
  }
  #if DEBUG_SYNC
  printf("sens %d:%d:%d adjusted. now %d:%d\n", proc_id, sys_id, sens_id,
          sens->cur, sens->max);
  #endif
}  

/* handle REM_SENS command, removes a sensitivity from the sync serv */
void handle_rem_sens(proces_id, system_id, paramc, paramv)
int proces_id; /* local proces_id of the source of the command */
int system_id; /* system_id of the source of the command */
int paramc; /* the number of parameters in the command */
int *paramv; /* pointer to the array of parameters */
{

  struct sensitivity *sens;
  int proc_id, sys_id, sens_id;
  proc_id = paramv[0];
  sys_id = paramv[1];
  sens_id = paramv[2];
  sens = &(glob_sens[proc_id][sys_id][sens_id]);
  sens->cur--;
  #if DEBUG_SYNC
  printf("sens %d:%d:%d adjusted. now %d:%d\n", proc_id, sys_id, sens_id,
          sens->cur, sens->max);
  #endif

}

/* handle SET_ALPHA command, add alphabet to a sensitvity */
void handle_set_alpha(proces_id, system_id, paramc, paramv)
int proces_id; /* local proces_id of the source of the command */
int system_id; /* system_id of the source of the command */
int paramc; /* the number of parameters in the command */
int *paramv; /* pointer to the array of parameters */
{
int i, proc_id, sys_id, sens_id;
  proc_id = paramv[0];
  sys_id = paramv[1];
  sens_id = paramv[2];
  (glob_sens[proc_id][sys_id][sens_id].max)++;
  #if DEBUG_SYNC
  printf("adjusted alphabet for %d:%d:%d, now %d\n", proc_id, sys_id, sens_id,
    (glob_sens[proc_id][sys_id][sens_id].max));
  #endif
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
  initialise(&glob_data, &glob_sens);
  printf("glob_sens: %x\n", glob_sens);
}
#endif
