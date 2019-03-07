#ifndef _SYNC_H_
#define _SUNC_H_

#define NUM_WISSELS 2
#define NUM_ONTKOPPEL 4
#define NUM_LOCOMOTIEF 2
#define NUM_SENSORS 1
#define NUM_PROCES_TYPES 5

#define SYSTEM_SENS NUM_PROCES_TYPES 
#define NUM_SYSTEM_SENS 1
#define TOCK 0

#define HDS_PROC SYSTEM_SENS /*index is shared with SYSTEM_SENS */

struct sensitivity {
  int max;
  int cur;
};

struct proces_data {
  int write_fd;
  int read_fd;
};

int get_num_procs(int proces_type);
int get_num_sens(int proces_type);
void print_sens(struct sensitivity ****sens);
char initialise(struct proces_data ***data, struct sensitivity ****sens);
#endif
