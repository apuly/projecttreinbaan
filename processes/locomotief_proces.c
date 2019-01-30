#include <treinbaan/locomotief.h>
#include <treinbaan/message.h>

#include <minix/ipc.h>

#define snel() send_msg(LOCOMOTIEF_SNEL)
#define langzaam() send_msg(LOCOMOTIEF_LANGZAAM)
#define stop() send_msg(LOCOMOTIEF_STOP)
#define vooruit() send_msg(LOCOMOTIEF_STOP)
#define achteruit() send_msg(LOCOMOTIEF_STOP)

long cs2_id;
struct exec_data data;

void recht();
void krom();


int init(void)
{
  int status;
  if (fgets((char *)&data.system_id, sizeof(int), stdin) != sizeof(int)){
    printf("Something went wrong while loading locomotief process!");
    return -1;
  }
  if (fgets((char *)&data.sync_pid, sizeof(int), stdin) != sizeof(int)){
    printf("Could not load pid for locomotief process %d!\n", data.system_id);
    return -1;;
  }
  if (fgets((char *)cs2_id, sizeof(long), stdin) != sizeof(long)){
    printf("Could not load cs2_id for locomotief %d!\n", data.system_id);
    return -1;
  }
  return OK; 
}

int ontvang_actie(void)
{
  message m;
  sef_receive(data.sync_pid, &m);
  if (m.TB_TARGET != data.system_id){
    return NOP;
  } else {
    return m.TB_CMD;
  }
}

PUBLIC int main(int argc, char **argv)
{
  int actie;
  if (!init()){
    return -1;
  }
  while (TRUE){
    actie = ontvang_actie();
    switch(actie)
    {
      case NOP:
        continue;
        break;
      case LOCOMOTIEF_SNEL:
        snel();
        stuur_sensitivity([LOCOMOTIEF_STOP, LOCOMOTIEF_LANGZAAM,
                           LOCOMOTIEF_SNEL, WFNP], 4);
        break;
      case LOCOMOTIEF_LANGZAAM:
        langzaam();
        stuur_sensitivity([LOCOMOTIEF_STOP, LOCOMOTIEF_LANGZAAM,
                           LOCOMOTIEF_SNEL, WFNP], 4);
        break;
      case LOCOMOTIEF_STOP:
        stop();
        stuur_sensitivity([LOCOMOTIEF_VOORUIT, LOCOMOTIEF_ACTERUIT, WFNP], 3);
        break;
      case LOCOMOTIEF_VOORUIT:
        vooruit();
        stuur_sensitivity([LOCOMOTIEF_SNEL, LOCOMOTIEF_LANGZAAM], 2);
        break;
      case LOCOMOTIEF_ACHTERUIT:
        achteruit();
        stuur_senstivity([LOCOMOTIEF_SNEL, LOCOMOTIEF_LANGZAAM], 2);


      default:
        print("locomotief %d received invalid command!\n", data.system_id);
        break;
    }
  }

  return -1; /* should never reach this code */
}

void send_msg(int msg){
  message m;
  m.TB_CMD = msg;
  m.TB_CSID = cs2_id;
  send(LOCOMOTIEF_PROC_NR, &m);
}
