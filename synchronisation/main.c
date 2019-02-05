#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "../includes/locomotief.h"
#include "../includes/ontkoppelaar.h"
#include "../includes/wissel.h"
#include "../includes/proces.h"

#define NUM_WISSELS 10
#define NUM_ONTKOPPEL 4
#define NUM_LOCOMOTIEF 2
#define NUM_SENSORS 1
#define NUM_PROCES_TYPES 4

#define NUM_WISSEL_SENSITIVITY 3
#define NUM_ONTKOPPEL_SENSITIVITY 2
#define NUM_LOCOMOTIEF_SENSITIVITY 6
#define NUM_SENSOR_SENSITIVITY 2

struct sensitivity {
  int max;  /* the total amount of processes that could be sensitive */
  int curr; /* the amount of processes that are currently sensitive */
};

struct proces_data {
  int write_fd;
  int read_fd;
  time_t last_read;
};

_PROTOTYPE(char initialise, (struct proces_data wissel_data[NUM_WISSELS],
                             struct proces_data ontkoppel_data[NUM_ONTKOPPEL], 
                             struct proces_data locomotief_data[NUM_LOCOMOTIEF],
                             struct proces_data sensor_data[NUM_SENSORS]
                                                                      ));

_PROTOTYPE(struct sensitivity **alloc_sens, (int num_procs, int num_sens));

_PROTOTYPE(void free_sens, (struct sensitivity **sens, int num_procs));

_PROTOTYPE( struct sensitivity ***init_alphabet, (void));

_PROTOTYPE( void free_alphabet, (struct sensitivity ***alphabet));

_PROTOTYPE( char init_process, (struct proces_data *data, char target_proces, 
                                  int proces_id));
_PROTOTYPE( int get_num_procs, (int proces_type));

_PROTOTYPE( char data_available, (struct proces_data *data));

_PROTOTYPE( void start_procs, (struct proces_data *data, int num_processes, 
                                   char proces));
_PROTOTYPE( void read_command, (int fd, int *command, int *param, int *paramc));


struct sensitivity **alloc_sens(int num_procs, int num_sens)
{
  int i;
  int ptr_size;
  struct sensitivity **out;
  ptr_size = sizeof(struct sensitivity **);
  out = malloc(num_procs*ptr_size);
  ptr_size = sizeof(struct sensitivity *);
  for (i=0; i<num_procs; i++){
    out[i] = malloc(num_sens*sizeof(struct sensitivity));
  }
}

void free_sens(struct sensitivity **sens, int num_procs)
{
  int i;
  if (sens != NULL){
    /*for (i=0; i<num_procs; i++){
      printf("freeing %d\t", i);
      free(sens[i]);
      printf("...freed!\n");
    }*/
    free(sens[0]);
    free(sens[5]);
  }
}

struct sensitivity ***init_alphabet(void)
{
  int i, j;
  int ptr_size;
  struct sensitivity ***procs;
  
  ptr_size = sizeof(struct sensitivity ***);
  procs = malloc(NUM_PROCES_TYPES*ptr_size);
  procs[WISSEL_PROCES] = alloc_sens(NUM_WISSELS, NUM_WISSEL_SENSITIVITY);
  procs[ONTKOPPEL_PROCES] = alloc_sens(NUM_ONTKOPPEL, NUM_ONTKOPPEL_SENSITIVITY);
  procs[LOCOMOTIEF_PROCES] = alloc_sens(NUM_LOCOMOTIEF, NUM_LOCOMOTIEF_SENSITIVITY);
  procs[SENSOR_PROCES] = alloc_sens(NUM_SENSORS, NUM_SENSOR_SENSITIVITY);
  return procs;
}

void free_alphabet(struct sensitivity ***alphabet)
{
  int i;
  if (alphabet != NULL){
    free_sens(alphabet[WISSEL_PROCES], NUM_WISSELS);
    free_sens(alphabet[LOCOMOTIEF_PROCES], NUM_LOCOMOTIEF);
    free_sens(alphabet[ONTKOPPEL_PROCES], NUM_ONTKOPPEL); 
    free_sens(alphabet[SENSOR_PROCES], NUM_SENSORS);
    printf("freed alphabets\n");
    free(alphabet);
    alphabet = NULL;
  }
}

char initialise(wissel_data, ontkoppel_data, locomotief_data, sensor_data)
struct proces_data wissel_data[NUM_WISSELS];
struct proces_data ontkoppel_data[NUM_ONTKOPPEL];
struct proces_data locomotief_data[NUM_LOCOMOTIEF];
struct proces_data sensor_data[NUM_SENSORS];
{
  printf("starting wissel processes!\n");
  start_procs(wissel_data, NUM_WISSELS, WISSEL_PROCES);
  printf("starting ontkoppel processes!\n");
  start_procs(ontkoppel_data, NUM_ONTKOPPEL, ONTKOPPEL_PROCES);
  printf("starting locomotief processes!\n");
  start_procs(locomotief_data, NUM_LOCOMOTIEF, LOCOMOTIEF_PROCES);
  printf("staring sensor processes!\n");
  start_procs(sensor_data, NUM_SENSORS, SENSOR_PROCES);
}

void start_procs(struct proces_data *data, int num_processes, char proces)
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
  struct stat s;
  printf("staring proces %d:%d\n", target_proces, proces_id);
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
    fstat(data->read_fd, &s);
    data->last_read = s.st_mtime;
    return 0;
  }
}

int get_num_procs(int proces_type)
{
  switch (proces_type){
    case WISSEL_PROCES:
      return NUM_WISSELS;
      break;
    case LOCOMOTIEF_PROCES:
      return NUM_LOCOMOTIEF;
      break;
    case ONTKOPPEL_PROCES:
      return NUM_ONTKOPPEL;
      break;
    case SENSOR_PROCES:
      return NUM_SENSORS;
      break;
    default:
      break;
  }
  return 0;
}

void synchronise(struct proces_data *data[NUM_PROCES_TYPES], struct sensitivity ***sens)
{
  int i, j, num_procs, paramc;
  int cmd, param[10];
  struct proces_data *data_ptr;
  while(1){
    for (i=0; i < NUM_PROCES_TYPES; i++){
      num_procs = get_num_procs(i);
      for (j=0; j<num_procs; j++){
        data_ptr = data[i] + j*sizeof(struct proces_data *);
        if (data_available(data_ptr)){
          read_command(data_ptr->read_fd, &cmd, param, &paramc);
          printf("received 'set alphabet' for %d:%d\n", i, j);
          /*read_data(char *command, char *params);*/
        }
      }
    }
  }
}

char data_available(struct proces_data *data)
{
  struct stat s;
  fstat(data->read_fd, &s);
  if (s.st_atime != data->last_read){
    data->last_read = s.st_atime;
    return true;
  } else {
    return false;
  }
}

void read_command(int fd, int *command, int *param, int *paramc)
{
  int len;
  read(fd, command, sizeof(int));
  read(fd, &len, sizeof(int));
  read(fd, param, len);
  *paramc = len;
}

int main(int argc, char **argv)
{
  struct proces_data wissel_data[NUM_WISSELS];
  struct proces_data ontkoppel_data[NUM_ONTKOPPEL];
  struct proces_data locomotief_data[NUM_LOCOMOTIEF];   
  struct proces_data sensor_data[NUM_SENSORS];

  struct proces_data *all_procs[NUM_PROCES_TYPES];
  
  struct sensitivity ***sens;
 
  all_procs[0] = wissel_data;
  all_procs[1] = ontkoppel_data;
  all_procs[2] = locomotief_data;
  all_procs[3] = sensor_data;

 
  printf("Allocating memory for alphabet!\n");
  sens = init_alphabet();
  printf("Initialising processes!\n");
  initialise(wissel_data, ontkoppel_data, locomotief_data, sensor_data);
  synchronise(all_procs, sens);
}

