#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../includes/locomotief.h"
#include "../includes/ontkoppelaar.h"
#include "../includes/wissel.h"
#include "../includes/proces.h"

#define NUM_WISSELS 10
#define NUM_ONTKOPPEL 4
#define NUM_LOCOMOTIEF 2
#define NUM_SENSORS 1
#define NUM_PROCES_TYPES 4

/*int alpha_ontkoppel[NUM_ONTKOPPEL];
int alpha_wissel[NUM_WISSELS];
int alpha_locomotief[NUM_LOCOMOTIEF];
int alpha_sensor[NUM_SENSORS];
int *alphabet[NUM_PROCES_TYPES] = {alpha_locomotief, alpha_wissel,
                                  alpha_ontkoppel, alpha_sensor};
*/




struct proces_data {
  int write_fd;
  int read_fd;
};

_PROTOTYPE(char initialise, (struct proces_data wissel_fd[NUM_WISSELS],
                             struct proces_data ontkoppel_fd[NUM_ONTKOPPEL], 
                             struct proces_data locomotief_fd[NUM_LOCOMOTIEF]
                                                                      ));
/*_PROTOTYPE(char start_processes, (struct proces_data data[], int num_processes,
                                  char proces                       ));
*/


_PROTOTYPE(char init_process, (struct proces_data *data,
                               char target_process,
                               int proces_id                          ));

int main(int argc, char **argv)
{
  struct proces_data wissel_data[NUM_WISSELS];
  struct proces_data ontkoppel_data[NUM_ONTKOPPEL];
  struct proces_data locomotief_data[NUM_LOCOMOTIEF];
  int status;
  
  status = initialise(wissel_data, ontkoppel_data, locomotief_data);
}

char initialise(wissel_data, ontkoppel_data, locomotief_data)
struct proces_data wissel_data[NUM_WISSELS];
struct proces_data ontkoppel_data[NUM_ONTKOPPEL];
struct proces_data locomotief_data[NUM_LOCOMOTIEF];
{
  printf("starting wissel processes!\n");
  start_procs(wissel_data, NUM_WISSELS, WISSEL_PROCES);
  printf("starting ontkoppel processes!\n");
  start_procs(ontkoppel_data, NUM_ONTKOPPEL, ONTKOPPEL_PROCES);
  printf("starting locomotief processes!\n");
  start_procs(locomotief_data, NUM_LOCOMOTIEF, LOCOMOTIEF_PROCES);
}

void start_procs(data, num_processes, proces)
struct proces_data *data;
int num_processes;
char proces;
{
  int i, status;
  for (i=0; i<num_processes; i++){
    status = init_process(&data[i], proces, i);
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
    close(to_child[0]);
    close(from_child[1]);
    
    child_data.read_fd = to_child[1];
    child_data.write_fd = from_child[0];
    child_data.system_id = proces_id;

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
    }
    exit(0);
  } else {
    close(to_child[1]);
    close(from_child[0]);
    data->write_fd = to_child[0];
    data->read_fd = from_child[1];
    return 0;
  }
}
