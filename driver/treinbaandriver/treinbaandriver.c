#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SOCKADDR struct sockaddr

#include <treinbaan/locomotief.h>
#include <treinbaan/ontkoppelaar.h>
#include <treinbaan/wissel.h>

#include "treinbaandriver.h" 

#define CAN_LENGTH 13
#define RSP_SEND 0
#define IPPORT_CS2_COMMAND 15731

long locomotief_ids[] = {0x0018, 0x4000};

long ontkoppelaar_ids[] = {0x300b, 0x300b, 0x300c, 0x300c};

long wissel_ids[] = {0x3000, 0x3001, 0x3002, 0x3003, 0x3004, 0x3005, 0x3006,
                    0x3007, 0x3008, 0x3009, 0x0030};

long lookup_target_ip(int system_id);
long translate_locomotief(int locomotief_id);
long translate_ontkoppelaar(int ontkoppelaar_id);
long translate_wissel(int wissel_id);
char translate_cmd(char cmd, int type, int system_id, struct cs2_cmd *command);
long translate_id(int type, int system_id);
int send_can_msg(const unsigned long ip, const char cmd,
                 const char *data, size_t data_length);
int send_command(const unsigned long ip, const char *cmd);
void hexdump(const char *buf, const size_t size);
int format_cmd_data(long addr, char *data, size_t data_len, char *buff);

void hexdump(const char *buf, const size_t size)
{
  unsigned addr;
  int i;
  unsigned char *p;

  for (addr=0; addr < size; addr+= 16){
    p = (unsigned char *)&buf[addr];
    printf("%04X: ", addr);
    i = size - addr;
    if (i > 16)
      i = 16;
    while(i--)
      printf("%02X ", *p++);
    printf("\n");
  }  
}

void send_cmd(int type, char cmd, int system_id)
{
  char buff[8]; /* 8 is max length for can bus command */
  size_t buff_len;
  char status;
  struct cs2_cmd *command;
  unsigned long ip;
  long cs2_id = translate_id(type, system_id);
  
  status = translate_cmd(cmd, type, system_id, command);
  if(status != 0){
    return;
  }
  ip = lookup_target_ip(system_id);
  /*printf("cmd: %d, p0: %d, p1: %d, p2: %d, paramc: %d\n", command->cmd, command->param[0], command->param[1], command->param[2], command->paramc);*/
  buff_len = format_cmd_data(cs2_id, command->param, command->paramc, buff);
  send_can_msg(ip, command->cmd, buff, buff_len);
}

int format_cmd_data(long addr, char *data, size_t data_len, char *buff)
{
  char *lp = (char *)&addr;
  memset(buff, 0, 8);
  buff[0] = lp[3];
  buff[1] = lp[2];
  buff[2] = lp[1];
  buff[3] = lp[0];
  memmove(buff+4, data, data_len);
  /*hexdump(buff, 8);*/
  return sizeof(long) + data_len;
}

/*start code from blackboard*/
int send_can_msg(const unsigned long ip, const char cmd,
                 const char *data, size_t data_length)
{
  char command[13];
  memset((void *)command, 0, CAN_LENGTH);

  command[0] = (char)(cmd >> 7);
  command[1] = (char)(cmd << 1 | RSP_SEND);
  command[2] = 0x03;
  command[3] = 0x00;
  if (!data || data_length > 8)
    data_length = 0;
  if (data_length)
    memmove(command+5, data, data_length);
  command[4] = (char)data_length;

  return send_command(ip, command);
}

int send_command(const unsigned long ip, const char *cmd)
{
  int sock_fd;
  struct sockaddr_in serv_addr;
  int c; /* result of connect() */
  int s; /* result of send()/sendto()/write() */
  
  sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (sock_fd == -1){
    printf("something went wrong while setting up sockets!\n");
    return 1;
  }

  memset((void *)&serv_addr, 0, sizeof serv_addr); /* not used, must be zero */
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(IPPORT_CS2_COMMAND);
  serv_addr.sin_addr.s_addr = ip;
  c = connect(sock_fd, (SOCKADDR *)&serv_addr, sizeof serv_addr);
  if (c < 0){
    printf("failed to set up connection!\n");
    return 2;
  }

  /*hexdump(cmd, CAN_LENGTH);*/

  s = send(sock_fd, cmd, CAN_LENGTH, 0);
  if (s < 0){
    printf("failed to send data to cs2!\n");
    return 3;
  }

  close(sock_fd);

  return 0;
}
/* end code from blackboard */


long lookup_target_ip(int system_id)
{
  return inet_addr(IP);
}

char translate_cmd(char cmd, int type, int system_id, struct cs2_cmd *command)
{
  memset(command, 0, sizeof(command));
  command->paramc = 2;
  switch(type){
    case WISSEL:
      command->cmd = 0x0b;
      command->param[0] = cmd == WISSEL_RECHT;
      command->param[1] = 1;
      /* bij param = 0 wordt de wissel op recht gezet
         bij param = 1 wordt de wissel op krom gezet */
      break;
    case ONTKOPPELAAR:
      command->cmd = 0x0b;
      command->param[0] = (system_id == 1 || system_id == 3) ? 1 : 0;
      command->param[1] = 1;
      /*
      id 2 & 4 zitten op het andere kanaal van de ontkoppelaars
      Deze moeten gecontroleerd worden om de juiste ontkoppelaar aan te sturen.
      */
      break;
    case LOCOMOTIEF:
      switch(cmd)
      {
        case LOCOMOTIEF_STOP:
          command->cmd = 0x04;
          command->param[0] = 0;
          break;
        case LOCOMOTIEF_LANGZAAM:
          command->cmd = 0x04;
          command->param[1] = 80;
          break;
        case LOCOMOTIEF_SNEL:
          command->cmd = 0x04;
          command->param[0] = 0x01;
          command->param[1] = 0x8F;
          break;
        case LOCOMOTIEF_VOORUIT:
          command->paramc = 1;
          command->cmd = 0x05;
          command->param[0] = 1;
          break;
        case LOCOMOTIEF_ACHTERUIT:
          command->paramc = 1;
          command->cmd = 0x05;
          command->param[0] = 2;
          break;
        default:
          printf("Command %d id invalid!\n", cmd);
          break;
      }
      break;
    default:
      printf("Invalid object type!\n");
      return -1;
  }
  return 0;
  
}

long translate_id(int type, int system_id)
/*translates a system_id to a cs2_id*/
{
  switch (type) {
    case LOCOMOTIEF:
      return translate_locomotief(system_id);
      break;
    case WISSEL:
      return translate_wissel(system_id);
      break;
    case ONTKOPPELAAR:
      return translate_ontkoppelaar(system_id);
      break;
    default:
      printf("can't translate system_id!\n");
      return 0;
  }
}

long translate_locomotief(int id)
{
  return locomotief_ids[id];
}

long translate_ontkoppelaar(int id)
{
  return ontkoppelaar_ids[id];
}

long translate_wissel(int id)
{
  return wissel_ids[id];
}

