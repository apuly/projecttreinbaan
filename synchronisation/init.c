#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "../includes/locomotief.h"
#include "../includes/ontkoppelaar.h"
#include "../includes/wissel.h"
#include "../includes/sensor.h"
#include "../includes/proces.h"
#include "../includes/sync.h"
#include "../includes/sensor_driver.h"
#include "../includes/trein_driver.h"
#include "../includes/rangeer.h"

_PROTOTYPE( char init_process, (struct proces_data *data, char target_proces, 
                                  int proces_id));

_PROTOTYPE( void start_procs, (struct proces_data *data, int num_processes, 
                                   char proces));

_PROTOTYPE( void init_proces_data, (struct proces_data ***data));

_PROTOTYPE( void init_sensitivity, (struct sensitivity ****sens_));

char initialise(struct proces_data ***data, struct sensitivity ****sens)
{
  int i;
  printf("Initialising synchronisation server!\n");
  init_proces_data(data);
  init_sensitivity(sens);
 
  /* start processes */
  for (i=0; i<NUM_PROCES_TYPES; i++){
    start_procs((*data)[i], get_num_procs(i), i);
  }

  return 0;
}

/*initialise the proces data array */
void init_proces_data(struct proces_data ***data)
{
  int i;
  int size;

  size = sizeof(struct proces_data *) * NUM_PROCES_TYPES;
  *data = malloc(size);
  for (i=0; i<NUM_PROCES_TYPES; i++){
    size = sizeof(struct proces_data) * get_num_procs(i);
    if (size != 0){
      (*data)[i] = malloc(size);
    }
    /*printf("%d, %d: %x\n", i, get_num_procs(i), (*data)[i]);*/
  }
}

/* initialise the sensitivity array */
void init_sensitivity(struct sensitivity ****sens)
{
  int i, j, num_sens, num_procs;
  const int spp = sizeof(struct sensitivity **); /* sensitivity pointer pointer size */
  const int sp = sizeof(struct sensitivity *); /* sensitivty pointer size */
  const int s = sizeof(struct sensitivity); /* sensitivity size */
  *sens = malloc(spp * NUM_PROCES_TYPES);
  for (i=0; i<NUM_PROCES_TYPES+1; i++){
    num_procs = get_num_procs(i);
    (*sens)[i] = malloc(sp * num_procs);
    for (j=0; j<num_procs; j++){
      num_sens = get_num_sens(i);
      (*sens)[i][j] = malloc(s*num_sens);
      memset((*sens)[i][j], 0, s*num_sens);
    }
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

/*forks the proces
children get an exec_data struct and are turned into appropriate prces
parent gets relevent data written to proces_data
*/
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
    
    child_data.read_fd = to_child[0]; /*build exec_data struct */
    child_data.write_fd = from_child[1];
    child_data.proces_id = target_proces;
    child_data.system_id = proces_id;

    switch (target_proces){ /* start proces as appropriate proces type */
      case WISSEL_PROCES:
        wissel_start(child_data);
        break;
      case ONTKOPPEL_PROCES:
        ontkoppel_start(child_data);
        break;
      case LOCOMOTIEF_PROCES:
        locomotief_start(child_data);
        break;
      case SENSOR_PROCES:
        sensor_start(child_data);
        break;
      case HDS_SENSOR:
        setup_sensorsub(child_data);
        break;
      case HDS_TREIN:
        treinbaan_start(child_data);
      case RANGEER_PROCES:
        rangeer_start(child_data);
        break;
      default:
        printf("target_proces: %d not found\n", target_proces);
    }
    exit(0);
  } else {
    close(to_child[0]);
    close(from_child[1]);

    data->write_fd = to_child[1]; /*build proces_data struct */
    data->read_fd = from_child[0];
    return 0;
  }
}

