#ifndef _TREINBAAN_PROCES_H_
#define _TREINBAAN_PROCES_H_

#define WISSEL_PROCES 0
#define ONTKOPPEL_PROCES 1
#define LOCOMOTIEF_PROCES 2
#define SENSOR_PROCES 3
#define HDS_SENSOR 4
#define HDS_TREIN 5
#define RANGEER_PROCES 6

#define KILL_PROCES 1 /* if set to 0, processes will terminate directly after init */

#define START_PROCES_WAIT_TIME 1 /* time in seconds between proces init and 
sending of sensitivity*/

struct exec_data { /* data for executing tasks */
  int read_fd; /* file discriptor to read from parent proces */
  int write_fd; /* file discriptor to write to parent proces */
  int proces_id; /* the identifier for the proces */
  int system_id; /* the id of the proces within the system */
};

/*void stuur_alphabet(struct exec_data *data, int *alpha, const int alpha_c);*/
void tock(struct exec_data data);

void send_sync_cmd(struct exec_data data, int cmd, int pid, int sid, int arg);

void send_alphabet(struct exec_data data, int proces_type,
                    int proces_id, int *alpha, const int alpha_c);


void send_alpha(struct exec_data data, int proc_id, int num_procs, int alpha_c);

int send_sensitivity(struct exec_data data, int *c_sens, int c_len,
                              int *n_sens, int n_len);
int receive_action(struct exec_data data, int *cmd, int *buff);
void untock(struct exec_data data);
void register_tock(struct exec_data data);
void set_sensitivity(struct exec_data data, int *sens, int sens_c);
void rem_sensitivity(struct exec_data data, int *sens, int sens_c);

#endif /* _TREINBAAN_PROCES_H_ */
