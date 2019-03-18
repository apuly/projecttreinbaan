/*
 * Demo:
 * Locomotief 1 vooruit.
 * Locomotief 1 langzaam.
 * Wacht op bepaalde sensor.
 * Ontkoppelaar ... hoog_laag.
 * Wacht op bepaalde sensor (of sleep(2))
 * Locomotief 1 stop.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../includes/rangeer.h"
#include "../includes/proces.h"
#include "../includes/debug.h"

#include "../includes/locomotief.h"
#include "../includes/wissel.h"
#include "../includes/ontkoppelaar.h"
#include "../includes/sensor.h"
#include "../includes/sync.h"
#include "../includes/command.h"
struct sens {
  int proc;
  int sys;
  int sens;
};

void send_alpha(struct exec_data data, int proc_id, int num_procs, int alpha_c);
struct sens update_sens(struct exec_data data, struct sens new_sens,
                        struct sens cur_sens);
void init_rangeer(struct exec_data data);
struct sens set_sens(int p_id, int sys_id, int sens_id);

void set_all(struct exec_data data, int sys_id, int sens_id);
void unset_all(struct exec_data data, int sys_id, int sens_id);

void send_sens(struct exec_data, int p_id, int sys_id, int sens_id);
void wait_for_sens(struct exec_data, struct sens);

void rangeer_start(struct exec_data data)
{
  int i = 0;
  unsigned int time = 0;
  struct sens c_sens;/*the current sensitivity*/
  init_rangeer(data);
  
  /* force the sleep */
  time = sleep(START_PROCES_WAIT_TIME);
  while(time){
    time = sleep(time);
  }
  /* correct any errors on the sensors */
  set_all(data, SENSOR_PROCES, LAAG);
  sleep(1);
  unset_all(data, SENSOR_PROCES, LAAG);


    send_sens(data, WISSEL_PROCES, 9, KROM);
    sleep(2);
    send_sens(data, WISSEL_PROCES, 9, RECHT);

  /* rijd achteruit voorbij de ontkoppelaar */
    send_sens(data, LOCOMOTIEF_PROCES, 0, ACHTERUIT);
    send_sens(data, LOCOMOTIEF_PROCES, 0, LANGZAAM);

    /*wacht op bepaalde sensor*/
    send_sens(data, SENSOR_PROCES, 5, HOOG);

    send_sens(data, LOCOMOTIEF_PROCES, 0, STOP);
    send_sens(data, LOCOMOTIEF_PROCES, 0, VOORUIT);
    send_sens(data, LOCOMOTIEF_PROCES, 0, LANGZAAM);

    send_sens(data, SENSOR_PROCES, 7, HOOG);
    send_sens(data, WISSEL_PROCES, 9, KROM);
    send_sens(data, ONTKOPPEL_PROCES, 3, HOOG_LAAG);

    send_sens(data, SENSOR_PROCES, 33, HOOG);
    send_sens(data, LOCOMOTIEF_PROCES, 0, STOP);
       while(1){

    }

}

struct sens update_sens(struct exec_data data, struct sens new_sens,
                        struct sens cur_sens)
{
  send_sync_cmd(data, REM_SENS, cur_sens.proc, cur_sens.sys, cur_sens.sens);
  send_sync_cmd(data, SET_SENS, new_sens.proc, new_sens.sys, new_sens.sens);
  return new_sens;
}

struct sens set_sens(int p_id, int sys_id, int sens_id)
{
  struct sens s_;
  s_.proc = p_id;
  s_.sys = sys_id;
  s_.sens = sens_id;
  return s_;
}

void init_rangeer(struct exec_data data)
{
  int type, num_proc, num_sens;
  /*iterate through all processes types, get their number of running processes,
    get the number of sensitivities, and mark their entire alphabet*/
  for(type=0; type<NUM_PROCES_TYPES; type++){
    num_proc = get_num_procs(type);
    num_sens = get_num_sens(type);
    #if DEBUG_RANGEER
    printf("%d: %d, %d\n", type, num_proc, num_sens);
    #endif
    send_alpha(data, type, num_proc, num_sens);
  }
}


void set_all(struct exec_data data, int proc_id, int sens_id){
  int num_proc, i;

  num_proc = get_num_procs(proc_id);

  for(i = 0; i<num_proc; i++){
    send_sync_cmd(data, SET_SENS, proc_id, i, sens_id);
  }
}

void unset_all(struct exec_data data, int proc_id, int sens_id){
  int num_proc, i;
  
  num_proc = get_num_procs(proc_id);

  for(i = 0; i<num_proc; i++){
    send_sync_cmd(data, REM_SENS, proc_id, i, sens_id);
  }
}

void send_sens(struct exec_data data, int p_id, int sys_id, int sens_id){
  struct sens s_;

  s_ = set_sens(p_id, sys_id, sens_id);

  send_sync_cmd(data, SET_SENS, p_id, sys_id, sens_id);
  wait_for_sens(data, s_);
  send_sync_cmd(data, REM_SENS, p_id, sys_id, sens_id);
  
}

void wait_for_sens(struct exec_data data, struct sens wait_sens){
  int size, cmd, buff[10], proc_id, sys_id, state;

  /* wait for the right sensitivity to be told */
  while(1){
    size = receive_action(data, &cmd, buff); /* recieve action from sync serv */
    if(cmd == HDS_ACTION){
      proc_id = buff[0];
      sys_id = buff[1];
      state = buff[2];
      if( 
         (wait_sens.proc == proc_id) &&
         (wait_sens.sys == sys_id  ) &&
         (wait_sens.sens == state  )
        ){
        #if DEBUG_RANGEER
          printf("done waiting on %d:%d:%d\n",proc_id, sys_id, state);
        #endif
        break;
      }
    } 
  }
  return;
}

