#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "../includes/proces.h"
#include "../includes/command.h"
#include "../includes/sync.h"

#define wrt_i(val) write(data->write_fd, &val, sizeof(int))
#define i_size sizeof(int)
_PROTOTYPE(void send_sync_cmd, (struct exec_data *data, int cmd, int pid,
                                int sid, int arg));

void send_sync_cmd(data, cmd, pid, sid, arg)
struct exec_data *data;
int cmd;
int pid;
int sid;
int arg;
{
  int args=3;
  wrt_i(cmd);
  wrt_i(args);
  wrt_i(pid);
  wrt_i(sid);
  wrt_i(arg);
}

void stuur_alphabet(data, alpha, alpha_c)
struct exec_data *data; /*child proces execution data, used for sending alpha*/
int *alpha; /* the alphabet for the proces */
const int alpha_c; /* the number of words in the alphabet */
{
  int i, cmd=SET_ALPHA, args=3;
  for (i=0; i<alpha_c; i++){
    send_sync_cmd(data, cmd, data->proces_id, data->system_id, alpha[i]);
  }
}

void set_sensitivity(data, sens, sens_c)
struct exec_data *data;
int *sens;
const int sens_c;
{
  int i, cmd=SET_SENS, args=3;
  for (i=0; i<sens_c; i++){
    send_sync_cmd(data, cmd, data->proces_id, data->system_id, sens[i]);
  }
}

void rem_sensitivity(data, sens, sens_c)
struct exec_data *data;
int *sens;
const int sens_c;
{
  int i, cmd=REM_SENS, args=3;
  for (i=0; i<sens_c; i++){
    send_sync_cmd(data, cmd, data->proces_id, data->system_id, sens[i]);
  }
}

void register_tock(data)
struct exec_data *data;
{
  int i, cmd=SET_ALPHA, args=3, pid=SYSTEM_SENS;
  send_sync_cmd(data, cmd, pid, 0, TOCK);
}


void tock(struct exec_data *data)
{
  int cmd = SET_SENS, args=3, pid=SYSTEM_SENS;
  send_sync_cmd(data, cmd, pid, 0, TOCK);
}


void untock(struct exec_data *data)
{
  int cmd = REM_SENS, args=3, pid=SYSTEM_SENS;
  send_sync_cmd(data, cmd, pid, 0, TOCK);
}
