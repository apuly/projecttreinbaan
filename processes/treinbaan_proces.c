#include <stdio.h>
#include <stdlib.h>

#include "../includes/debug.h"
#include "../includes/command.h"
#include "../includes/proces.h"
#include "../driver/treinbaandriver/treinbaandriver.h"

void exec_treinbaan(int proc_id, int sys_id, int sens_id);

void treinbaan_start(struct exec_data data)
{
  int size, cmd, buff[10], proc_id, sys_id, sens_id;
  while (1){
    size = receive_action(data, &cmd, buff);
    if (cmd == HDS_ACTION){
      proc_id = buff[0];
      sys_id = buff[1];
      sens_id = buff[2];
      if (proc_id == ONTKOPPEL_PROCES || proc_id == LOCOMOTIEF_PROCES ||
          proc_id == WISSEL_PROCES){
        exec_treinbaan(proc_id, sys_id, sens_id);
      }
    } else if (cmd == EXIT) {
      exit(0);
    }
  }
}

void exec_treinbaan(int proc_id, int sys_id, int sens_id)
{
  #if DEBUG_TREINBAAN
  printf("treinbaan: %d:%d:%d\n", proc_id, sys_id, sens_id);
  #endif
  send_cmd(proc_id, (char)(sens_id&0xff), sys_id);
}
