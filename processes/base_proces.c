#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "../includes/proces.h"
#include "../includes/command.h"
#include "../includes/sync.h"

#define wrt_i(val) write(data.write_fd, &val, sizeof(int))
#define i_size sizeof(int)

/* send a command to the synchronisation server */
void send_sync_cmd(data, cmd, pid, sid, arg)
struct exec_data data;
int cmd; /* the command id */
int pid; /* the local proces id of the targeted proces */
int sid; /* the system id of the targeted proces */
int arg; /* the command arguments */
{
  int args=3;
  wrt_i(cmd);
  wrt_i(args);
  wrt_i(pid);
  wrt_i(sid);
  wrt_i(arg);
}

/*send the alphabet of a proces to the synchronisation server */
void send_alphabet(data, proces_type, proces_id, alpha, alpha_c)
struct exec_data data; /*child proces execution data, used for sending alpha*/
int proces_type; /* type of the proces, as defined in includes/sync.h*/
int proces_id; /* id of the proces. used as index in sensitivity list.
                  generally system_id that is given in proces init */
int *alpha; /* the alphabet for the proces */
const int alpha_c; /* the number of words in the alphabet */
{
  int i, cmd=SET_ALPHA;
  for (i=0; i<alpha_c; i++){
    send_sync_cmd(data, cmd, proces_type, proces_id, alpha[i]);
  }
}

/* tell the synchronisation server what a proces is sensitivity to */
void set_sensitivity(data, sens, sens_c)
struct exec_data data;
int *sens; /* pointer to array of sensitivites that need to be added */
const int sens_c; /* size of sens */
{
  int i, cmd=SET_SENS;
  for (i=0; i<sens_c; i++){
    send_sync_cmd(data, cmd, data.proces_id, data.system_id, sens[i]);
  }
}

/* tell the synchronisation server what a proces is no longer sensitive to */
void rem_sensitivity(data, sens, sens_c)
struct exec_data data;
int *sens; /* pointer to array of sensitivities that need to be removed */
const int sens_c; /* the size of sens */
{
  int i, cmd=REM_SENS;
  for (i=0; i<sens_c; i++){
    send_sync_cmd(data, cmd, data.proces_id, data.system_id, sens[i]);
  }
}

/* tell the synchronisation server that a proces is sensitivete to TOCK
   TOCK being the sensitivity that is used for synchronisation */
void register_tock(data) 
struct exec_data data;
{
  int i, cmd=SET_ALPHA, pid=SYSTEM_SENS;
  send_sync_cmd(data, cmd, pid, 0, TOCK);
}

/* tell the synchronisation server that a proces is currently sensitive to TOCK */
void tock(struct exec_data data)
{
  int cmd = SET_SENS, pid=SYSTEM_SENS;
  send_sync_cmd(data, cmd, pid, 0, TOCK);
}

/* tell the synchronisation server that a proces is no longer sensitive to TOCK */
void untock(struct exec_data data)
{
  int cmd = REM_SENS, pid=SYSTEM_SENS;
  send_sync_cmd(data, cmd, pid, 0, TOCK);
}

/* remove the old sensitivite of a proces and send the new one to the
synchronisation server */
int send_sensitivity(data, c_sens, c_len, n_sens, n_len)
struct exec_data data;
int *c_sens; /* array of current sensitivityies */
int c_len; /* length of c_sens */
int *n_sens; /* array of new sensitivities */
int n_len; /* length of n_sens */
{
  rem_sensitivity(data, c_sens, c_len);
  set_sensitivity(data, n_sens, n_len);
  memcpy(c_sens, n_sens, n_len*sizeof(int));
  return n_len;
}

int receive_action(data, cmd, buff)
struct exec_data data; /* struct of process where the read fd is stored */
int *cmd; /* buffer where the command id will be written to */
int *buff; /* buffer where the command parameters will be written to */
{
  /*read data from SynchronisationServer pipe*/
  /*return the size of the param buffer*/
  int len;
  int error;
  error = read(data.read_fd, cmd, sizeof(int));

  /* error check for nonblocking pipes */
  if(error == -1){
    if(errno == EAGAIN){  /* if the pipe is empty */
      cmd = NO_ACTION;    /* set the comand to no action */
      len = 0;            /* set len to 0, because there will be no data in the buffer */
      return len;           /* then return len */
    }
  }
#if DEBUG_SYNC 
  printf("cmd: %d;", *cmd);
#endif
  read(data.read_fd, &len, sizeof(int));
#if DEBUG_SYNC
  printf("len: %d\n", len);
#endif
  read(data.read_fd, buff, len*sizeof(int));
  return len;
}
