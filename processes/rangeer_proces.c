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

/*
 * Demo:
 * Locomotief 1 vooruit.
 * Locomotief 1 langzaam.
 * Wacht op bepaalde sensor.
 * Ontkoppelaar ... hoog_laag.
 * Wacht op bepaalde sensor (of sleep(2))
 * Locomotief 1 stop.
 */

void rangeer_start(struct exec_data data)
{
  int i = 0;
  struct sens c_sens;/*the current sensitivity*/
  init_rangeer(data);
  
  sleep(START_PROCES_WAIT_TIME);
  c_sens = set_sens(WISSEL_PROCES, 0, KROM);
  send_sync_cmd(data, SET_SENS, c_sens.proc, c_sens.sys, c_sens.sens);
  while(1){
    sleep(2);
    c_sens = update_sens(data, set_sens(LOCOMOTIEF_PROCES, 0, VOORUIT), c_sens);
    sleep(2);
    c_sens = update_sens(data, set_sens(LOCOMOTIEF_PROCES, 0, LANGZAAM), c_sens);
    sleep(2);
	/*wacht op bepaalde sensor*/
	/*c_sens = update_sens(data, set_sens(ONTKOPPEL_PROCES, ?, HOOG_LAAG), c_sens);*/
	/*sleep(2);*/
    c_sens = update_sens(data, set_sens(LOCOMOTIEF_PROCES, 0, STOP), c_sens);
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

