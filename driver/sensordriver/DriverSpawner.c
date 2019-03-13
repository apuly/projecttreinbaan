/*
   this program will probably give recieve a pipe when its created from the sync server
   (but i'm not going to worry about that yet)

   this program should do the following:
   create two pipes for comunitaction with the driver,
   fork off a child process,
   make that child execve into the driver, and let it take its pipes with it
   (that way the driver can do its thing, and send information back to the event handler)

   then either this program wil execve into the event handler program, or it will just start running that code directly

 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>

#include "linkedlist.h"

#include "../../includes/proces.h"
#include "../../includes/command.h"
#include "../../includes/debug.h"


struct sensorupdate* read_driver(int driver_pipe);

#define standaloneSensor 0 
#define alpha_c (sizeof(ALPHABET) / sizeof(int))

int send_update(struct exec_data data, struct sensorupdate* SensUpdate);

#if standaloneSensor
void sensor_event_start(/*int sync_pipe,*/ int driver_pipe);


int main(int argc, char *argv[], char *env[])
#else
  void sensor_event_start(struct exec_data, int driver_pipe, pid_t driver_pid);


void setup_sensorsub(struct exec_data data)
#endif
{
  /* variables */
  int driverPipe[2];
  pid_t pid;
  int error;

  char minibuf[100];

  printf("sensor subsystem started\n");

  /* open up a pipe */
  error = pipe(driverPipe);
  if (error == -1)
  {
    perror("Sensor Hds: error on pipe creation");
    exit(1);
  }

  pid = fork();
  if (pid == -1)
  {
    perror("Sensor Hds: error on fork");
    exit(1);
  }

  if (pid == 0)
  { /*child process*/
    char * noarg[] = {NULL};
    /* closes output side of the pipe */
    close(driverPipe[0]);
    /* redirect stdout to pipe */
    error = dup2(driverPipe[1], 1);
    if (error == -1)
    {
      perror("Sensor Hds: dup failed...");
    }
    /* replace child process with the driver */
    error = execve("sensdriver", noarg, noarg);

    if(error == -1){
      perror("Sensor Hds: execve failed");
    }
    /* if this code gets to run, that means the execve failed */
    printf("execve failed :(\n");
    exit(2);
  }
  else
  { /* parent process */
    /* closes input side of the pipe */
    close(driverPipe[1]);

    /* set the recieving end to nonblocking */
    error = fcntl(driverPipe[0], F_SETFL, O_NONBLOCK);
    if(error == -1)
    {
      perror("Sensor Hds Driver: failed to set read end to nonblock");
    }
#if standaloneSensor
    sensor_event_start(/*sync_pipe,*/ driverPipe[0]);
#else
    /* set the recieve pipe from the sync server to nonblocking */
    error = fcntl(data.read_fd, F_SETFL, O_NONBLOCK);
    sensor_event_start(data, driverPipe[0], pid);
#endif
  }
  exit(0);
}
#if standaloneSensor
void sensor_event_start(/*int sync_pipe,*/ int driver_pipe)
#else
void sensor_event_start(struct exec_data data, int driver_pipe, pid_t driver_pid)
#endif
{
  char inBuff[sizeof(int16_t)];
  int16_t val = 0;
  int error;
  int cmd, buff[10];

  while (KILL_PROCES)
  {
    size_t number = 0;
    struct sensorupdate* SensorUpdate;
    /* the event handler checks to see if the driver has sent it a message */
    SensorUpdate = read_driver(driver_pipe);
    if(NULL != SensorUpdate){
    send_update(data, SensorUpdate);
    }
#if standaloneSensor == 0
    /* check if we got a kill comand from the server */
    receive_action(data, &cmd, buff);
    /* the only action we need to act on is the exit action */
    if(cmd == EXIT){
      int error = kill(driver_pid, SIGTERM);
      if(error == -1){
        perror("sensor subsystem: driver doesn't want to exit peacefully");
        error = kill(driver_pid, SIGKILL);
        if(error == -1){
          perror("sensor subsystem: bruteforce doesn't work either... god have mercy");
        }else{
          printf("sensor subsystem: bruteforce worked though\n");
        }
      }
      exit(0);
    }

    /* then it prepares a sensitivity list to send to the sync server */
    /* then it sends the sensitivities */
#endif
  }
}

struct sensorupdate* read_driver(int driver_pipe)
{
  static struct sensorupdate TMPelement;
  int nread;

  nread = read(driver_pipe, &TMPelement, sizeof(struct sensorupdate));
  /*    printf("number of bytes read: %d\n", nread);*/
  switch(nread){
    case -1:
      if(errno == EAGAIN){
        /*if this piece of code gets hit, then there is no data on the pipe and no action has to be taken */
      }else{
        perror("Sensor Hds: error on read from pipe");
        exit(3);
      }
      break;
    case 0:
#if DEBUG_HDS_SENSOR
      printf("Sensor Hds: eof reached\nShutting down Sensor Hds");
      exit(1);
#endif
      break;
    default:
#if DEBUG_HDS_SENSOR
      printf("recieved from pipe:\n");
      printf("Sensor Number: %hd\n Sensor State: %hd\n", TMPelement.sensor, TMPelement.state);
#endif
      return &TMPelement;
      break;
  }
  return NULL;
}

int send_update(struct exec_data data, struct sensorupdate* SensUpdate)
{
  /*sensitivity arrays, needed for send_sensitivity */
  int new_sens, curr_sens;
  int size, cmd;

  int sens_num = (int)(SensUpdate->sensor);
  int sens_state = (int)(SensUpdate->state);

  /*  check the current state, this is the new sensitivity
      the old sensitivity is !sensState
   */
  if(sens_state){
    new_sens = 1;
    curr_sens = 0;
  }else{
    new_sens = 0;
    curr_sens = 1;
  }

  /* now what we have to do is send the new sensitivity */
  /* might be best to call send_sync_cmd directly */ 
  /* remove old sensitivity */
  send_sync_cmd(data, REM_SENS, sens_num, SENSOR_PROCES, curr_sens);
  /* set new sensitivity */
  send_sync_cmd(data, SET_SENS, sens_num, SENSOR_PROCES, new_sens);

  printf("sensor sub: sensitivity set\n");
  fflush(stdout);
}

