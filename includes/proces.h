#ifndef _TREINBAAN_PROCES_H_
#define _TREINBAAN_PROCES_H_

struct exec_data { /* data for executing tasks */
  int sync_pid; /* PID van de synchronisatie server */
  int system_id; /* lokaal ID voor communicatie met synchronisatie server */
  int fd[2]; /*Input and Output descriptor*/
};

int init(void);
void stuur_sensitivity(int *sensitivity, int num_sens);
int ontvang_actie(void);

/*
_PROTOTYPE( int init, (void));
_PROTOTYPE( void stuur_sensitivity, (int *sensitivity, int num_sens));
_PROTOTYPE( int ontvang_actie, (void)); 
*/

#endif /* _TREINBAAN_PROCES_H_ */
