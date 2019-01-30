#ifndef _TREINBAAN_DRIVER_H_
#define _TREINBAAN_DRIVER_H_

#define IP "192.168.1.53"

#define WISSEL 0
#define ONTKOPPELAAR 1
#define LOCOMOTIEF 2

struct cs2_cmd{
  char cmd;
  char param[3];
  char paramc;
};

void send_cmd(int type, char cmd, int addr);
/*
long locomotief_ids[] = {0x0024, 0x3000};

long ontkoppelaar_ids[] = {0x300c, 0x300c, 0x300d, 0x300d};

long wissel_ids[] = {0x3000, 0x3001, 0x3002, 0x3003, 0x3004, 0x3005, 0x3006,
                    0x3007, 0x3008, 0x3009};
*/
#endif /* _TREINBAAN_DRIVER_H_ */
