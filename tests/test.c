#include "treinbaandriver.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <treinbaan/wissel.h>
#include <treinbaan/ontkoppelaar.h>
#include <treinbaan/locomotief.h>

#define cmp(s1, s2) (strcmp(s1, s2) == 0)

int main(int argc, char **argv)
{
  int id;
  int cmd;
  if (argc < 3){
    printf("invalid amount of arguments!\n");
    printf("arg1: [wissel|ontkoppelaar|locomotief]\n");
    printf("arg2: system_id\n");
    return -1;
  } else {
    id = atoi(argv[2]);

    if (cmp(argv[1], "wissel")){
      if (argc != 4){
        printf("invalid argument. 3rd param must be 'recht' or anything else\n");
        return -1;
      }
      cmd = cmp(argv[3], "recht") ? WISSEL_RECHT : WISSEL_KROM;
      send_cmd(WISSEL, cmd, id);
    } else if (cmp(argv[1], "ontkoppelaar")){
      cmd = ONTKOPPELAAR_HOOG_LAAG;
      send_cmd(ONTKOPPELAAR, cmd, id);
    } else if (cmp(argv[1], "locomotief")){
      if (cmp(argv[3], "vooruit")){
        send_cmd(LOCOMOTIEF, LOCOMOTIEF_VOORUIT, id);
      } else if (cmp(argv[3], "achteruit")){
        send_cmd(LOCOMOTIEF, LOCOMOTIEF_ACHTERUIT, id);
      } else if (cmp(argv[3], "stop")){
        send_cmd(LOCOMOTIEF, LOCOMOTIEF_STOP, id);
      } else if (cmp(argv[3], "snel")){
        send_cmd(LOCOMOTIEF, LOCOMOTIEF_SNEL, id);
      } else if (cmp(argv[3], "langzaam")){
        send_cmd(LOCOMOTIEF, LOCOMOTIEF_LANGZAAM, id);
      } else {
        printf("WARNING: %s is invalid command!");
      }
    }
  }

  return 0;
}
