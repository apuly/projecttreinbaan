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

void stuur_sensitivity(int *sensitivity, int num_sens);
int ontvang_actie(void);


#endif /* _TREINBAAN_PROCES_H_ */
