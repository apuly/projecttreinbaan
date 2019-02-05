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

_PROTOTYPE( char data_available, (struct proces_data *data));

_PROTOTYPE( void start_procs, (struct proces_data *data, int num_processes, 
                                   char proces));
_PROTOTYPE( void read_command, (int fd, int *command, int *param, int *paramc));

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
  printf("reading command");
  read(fd, command, sizeof(int));
  printf("... read command %d;\nreading param_c", *command);
  read(fd, &len, sizeof(int));
  printf("... read paramc %d;\nreading params", len);
  for (i=0; i<len; i++){
    read(fd, param+i, sizeof(int));
  }
  *paramc = len;
}

/*
int main(int argc, char **argv)
{
  struct proces_data w_data[NUM_WISSELS];
  struct proces_data o_data[NUM_ONTKOPPEL];
  struct proces_data l_data[NUM_LOCOMOTIEF];
  struct proces_data s_data[NUM_SENSORS];
  struct proces_data *data[4];
  data[LOCOMOTIEF_PROCES] = l_data;
  data[ONTKOPPEL_PROCES] = o_data;
  data[WISSEL_PROCES] = w_data;
  data[SENSOR_PROCES] = s_data;
  initialise(data);
} */ 

int main(int argc, char **argv)
{
  struct proces_data data;
  int command, param_buff[10], paramc, i;
  init_process(&data, LOCOMOTIEF_PROCES, 0);
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
  /*
  initialise(wissel_data, ontkoppel_data, locomotief_data, sensor_data);
  synchronise(all_procs, sens);*/
}

