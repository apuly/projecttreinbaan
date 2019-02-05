#include "../includes/proces.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

void stuur_alphabet(data, alpha, alpha_c)
struct exec_data *data; /*child proces execution data, used for sending alpha*/
int *alpha; /* the alphabet for the proces */
const int alpha_c; /* the number of words in the alphabet */
{
  int *buff = malloc(sizeof(int)*2 + sizeof(int)*alpha_c);
  buff[0] = 1;
  buff[1] = alpha_c;
  memmove((char*)(buff+2), (char *)alpha, alpha_c*sizeof(int));
  write(data->write_fd, buff, sizeof(buff));
  free(buff);
}
