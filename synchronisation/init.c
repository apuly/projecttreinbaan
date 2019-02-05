#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "../includes/locomotief.h"
#include "../includes/ontkoppelaar.h"
#include "../includes/wissel.h"
#include "../includes/sensor.h"
#include "../includes/proces.h"
#include "../includes/sync.h"

_PROTOTYPE( char init_process, (struct proces_data *data, char target_proces, 
                                  int proces_id));

_PROTOTYPE( void start_procs, (struct proces_data *data, int num_processes, 
                                   char proces));

char initialise(struct proces_data **data)
{
  int i;
  for (i=0; i<NUM_PROCES_TYPES; i++){
    printf("starting proces type --%d--\n", i);
    start_procs(data[i], get_num_procs(i), i);
  }
}

void start_procs(struct proces_data *data, int num_processes, char proces)
{
  int i, status;
  for (i=0; i<num_processes; i++){
    status = init_process(data+i, proces, i);
    if (status != 0){
      printf("something went wrong while starting %s process!\n", proces);
    }
  }
}

char init_process(struct proces_data *data, char target_proces, int proces_id)
{
  char s1, s2;
  int pid;
  int to_child[2], from_child[2];
  struct exec_data child_data;
  struct stat s;
  printf("starting proces %d:%d\n", target_proces, proces_id);
  if (pipe(to_child) != 0 || pipe(from_child) != 0){
    /*check for errors while creating pipes */
    printf("Something went wrong while setting up pipes!\n");
    return -1;
  } 

  pid = fork();
  if (pid < 0){
    return -1;
  } else if (pid == 0){
    /* the child proces */
    close(to_child[1]);
    close(from_child[0]);
    
    child_data.read_fd = to_child[0];
    child_data.write_fd = from_child[1];

    switch (target_proces){
      case WISSEL_PROCES:
        wissel_start(&child_data);
        break;
      case ONTKOPPEL_PROCES:
        ontkoppel_start(&child_data);
        break;
      case LOCOMOTIEF_PROCES:
        locomotief_start(&child_data);
        break;
      case SENSOR_PROCES:
        sensor_start(&child_data);
        break;
    }
    exit(0);
  } else {
    close(to_child[0]);
    close(from_child[1]);
    data->write_fd = to_child[1];
    data->read_fd = from_child[0];
    fstat(data->read_fd, &s);
    data->last_read = s.st_mtime;
    return 0;
  }
}

