#include <treinbaan/sensor.h>
#include <treinbaan/message.h>

#include <minix/ipc.h>

int sens_id;

int init(void)
{
  int status;
  if (fgets((char *)&data.system_id, sizeof(int), stdin) != sizeof(int)){
    printf("Something went wrong while loading sensor process!");
    return -1;
  }
  if (fgets((char *)&data.sync_pid, sizeof(int), stdin) != sizeof(int)){
    printf("Could not load pid for sensor process %d!\n", data.system_id);
    return -1;;
  }
  if (fgets((char *)sens_id, sizeof(long), stdin) != sizeof(long)){
    printf("Could not load sens_id for sensor %d!\n", data.system_id);
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
    char state;
    /*initalize proces*/
    if (!init()){
        /* hi there, im michael jordan. */
        /* if the init of this system fails */
        /* stop it, get some help */
        /* https://www.youtube.com/watch?v=5vk7GLP4GFk */
        return -1;
    }
    while (TRUE){
        actie = ontvang_actie();
        switch(actie)
        {
            case NOP:
                continue;
                break;
            case GET_SENSOR:
                state = getstate();
                stuur_sensitivity([GET_SENSOR, WFNP], 1);
                break;
            default:
                print("sensor %d recieved invalid command!\n", data.system_id);
                break;
        }
    }
    
    return -1; /* you should never reach this code */
}

char getstate(void){
    char state;
    message m_ret;
    int status;

    /* send the request for data out */
    message m_send;
    m_send.TB_CMD = GET_STATE;
    send(SENSOR_PROC_ID, &m_send);

    /* wait for the driver to respond with your state */
    status = sef_recieve(ANY_ &m_ret);
    if(OK != status){
        panic("sensor failed to recieve state!: %d", )
    }
    /* TODO: implementation of this should still be done */
    state = m_ret->SENSOR_STATE;
    return state;
}
