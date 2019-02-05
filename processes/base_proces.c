#include "../includes/proces.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define wrt_i(val) write(data->write_fd, &val, sizeof(int))
#define i_size sizeof(int)

void stuur_alphabet(data, alpha, alpha_c)
struct exec_data *data; /*child proces execution data, used for sending alpha*/
int *alpha; /* the alphabet for the proces */
const int alpha_c; /* the number of words in the alphabet */
{
  int i, cmd = 1;
  wrt_i(cmd);
  wrt_i(alpha_c);
  for(i=0; i<(alpha_c); i++){
    wrt_i(alpha[i]);
  }
}
