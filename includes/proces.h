#ifndef _TREINBAAN_PROCES_H_
#define _TREINBAAN_PROCES_H_

#define LOCOMOTIEF_PROCES 0
#define WISSEL_PROCES 1
#define ONTKOPPEL_PROCES 2
#define SENSOR_PROCES 3

struct exec_data { /* data for executing tasks */
  int system_id; /* lokaal ID voor communicatie met synchronisatie server */
  int read_fd; /* file discriptor to read from parent proces */
  int write_fd; /* file discriptor to write to parent proces */
};

void stuur_alphabet(struct exec_data *data, int proces_type,
                                 int proces_id, int *alpha,
                                 const int alpha_c);

void change_sens(int action, int *new_sensitivity);
void send_sensitivity(int *current_sensitivity, int *new_sensitivity, struct exec_data *data);
/*void send_sensitivity(int *sensitivity, int num_sens);*/ /*waar is int num_sens voor nodig?*/
char *receive_action(char *action, struct exec_data *data);


#endif /* _TREINBAAN_PROCES_H_ */
