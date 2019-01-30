#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include <treinbaan/proces.h> 
#include "rangeer.h"
#include "start_up_check.h"

#define AANTAL_PROCESSEN 19

int main(int argc, char *argv[], char *env[])
{
	struct exec_data children[AANTAL_PROCESSEN];
	int fd[2];
	pid_t pid;
	int i;
	char buf[256];
	
        start_up_check();

	for(i = 0; i < (AANTAL_PROCESSEN+1); i++)
	{
		pipe(fd);
		
		pid = fork();
		if (pid < 0) 				/*Fork failed*/
			exit(EXIT_FAILURE);
		else if (pid > 0)			/*Parent process*/
		{	
			close(fd[1]); /*Close output side*/
			children[i].sync_pid = pid;
			children[i].system_id = i;
			children[i].fd[0] = fd[0];
			children[i].fd[1] = fd[1];
		}
		else 						/*Child process*/
		{
			close(fd[0]); /*Close input side*/
                       	if(i == 0) /*rangeer*/
			{
				rangeer();
			}
			else if(i < 3) /*Locomotief*/
			{
				/*roep loco functie aan...*/
			}
			else if(i < 6) /*Ontkoppelaar*/
			{
				/*roep ontkoppel functie aan...*/
			}
			else if(i < 16) /*Wissel*/
			{
				/*roep wissel functie aan...*/
			}
		}
		
	}
	
	read(children[AANTAL_PROCESSEN].fd[0], buf, 256);
	printf("Comm server %s id = %d\n", buf, children[4].system_id);
	
	return 0;
}





