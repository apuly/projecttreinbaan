#include <treinbaan/ontkoppelaar.h>
#include <treinbaan/message.h>

#include <minix/ipc.h>

long cs2_id;
struct exec_data data;

void recht();
void krom();


int init(void)
{
  int status;
  if (fgets((char *)&data.system_id, sizeof(int), stdin) != sizeof(int)){
    printf("Something went wrong while loading ontkoppelaar process!");
    return -1;
  }
  if (fgets((char *)&data.sync_pid, sizeof(int), stdin) != sizeof(int)){
    printf("Could not load pid for ontkoppelaar process %d!\n", data.system_id);
    return -1;;
  }
  if (fgets((char *)cs2_id, sizeof(long), stdin) != sizeof(long)){
    printf("Could not load cs2_id for ontkoppelaar %d!\n", data.system_id);
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
      case ONTKOPPELAAR_HOOG_LAAG:
        hoog_laag();
        stuur_sensitivity([ONTKOPPELAAR_HOOG_LAAG, WFNP], 2);
        break;
      default:
        print("ontkoppelaar %d received invalid command!\n", data.system_id);
        break;
    }
  }

  return -1; /* should never reach this code */
}

void hoog_laag(void){
  message m;
  m.TB_CMD = ONTKOPPELAAR_HOOG_LAAG;
  send(ONTKOPPELAAR_PROC_ID, &m);
}
