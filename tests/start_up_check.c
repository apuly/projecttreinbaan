#include "driver/treinbaandriver.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <treinbaan/wissel.h>
#include <treinbaan/ontkoppelaar.h>
#include <treinbaan/locomotief.h>

#include "start_up_check.h" 

#define locomotief0(actie) send_cmd(LOCOMOTIEF, actie, 0)
#define locomotief1(actie) send_cmd(LOCOMOTIEF, actie, 1)

#define wissel0(actie)     send_cmd(WISSEL, actie, 0) 					
#define wissel1(actie)	   send_cmd(WISSEL, actie, 1)
#define wissel2(actie)	   send_cmd(WISSEL, actie, 2)
#define wissel3(actie)	   send_cmd(WISSEL, actie, 3)
#define wissel4(actie)	   send_cmd(WISSEL, actie, 4)
#define wissel5(actie)	   send_cmd(WISSEL, actie, 5)
#define wissel6(actie)	   send_cmd(WISSEL, actie, 6)
#define wissel7(actie)	   send_cmd(WISSEL, actie, 7)
#define wissel8(actie)	   send_cmd(WISSEL, actie, 8)
#define wissel9(actie)	   send_cmd(WISSEL, actie, 9)

#define ontkoppelaar0() send_cmd(ONTKOPPELAAR, ONTKOPPELAAR_HOOG_LAAG, 0)
#define ontkoppelaar1() send_cmd(ONTKOPPELAAR, ONTKOPPELAAR_HOOG_LAAG, 1)
#define ontkoppelaar2() send_cmd(ONTKOPPELAAR, ONTKOPPELAAR_HOOG_LAAG, 2)
#define ontkoppelaar3() send_cmd(ONTKOPPELAAR, ONTKOPPELAAR_HOOG_LAAG, 3)

#define SEC1 (1)

void do_check(void);

int start_up_check()
{
	while(1){
		do_check();
		printf("rerun the check? y\n");
		if (getchar() != 'y'){
			return 0;
		}
	}
}

void do_check()
{
        #define WISSEL_SLEEP 300000
	printf("executing selfcheck!\n");
	
	printf("Checking wissels!\n");
	
	wissel0(WISSEL_KROM);
	usleep(WISSEL_SLEEP);
	wissel1(WISSEL_KROM);
	usleep(WISSEL_SLEEP);
	wissel2(WISSEL_KROM);
	usleep(WISSEL_SLEEP);
	wissel3(WISSEL_KROM);
	usleep(WISSEL_SLEEP);
	wissel4(WISSEL_KROM);
	usleep(WISSEL_SLEEP);
	wissel5(WISSEL_KROM);
	usleep(WISSEL_SLEEP);
	wissel6(WISSEL_KROM);
	usleep(WISSEL_SLEEP);
	wissel7(WISSEL_KROM);
	usleep(WISSEL_SLEEP);
	wissel8(WISSEL_KROM);
	usleep(WISSEL_SLEEP);
	wissel9(WISSEL_KROM);
	
        sleep(SEC1);
	
	wissel0(WISSEL_RECHT);
	usleep(WISSEL_SLEEP);
	wissel1(WISSEL_RECHT);
	usleep(WISSEL_SLEEP);
	wissel2(WISSEL_RECHT);
	usleep(WISSEL_SLEEP);
	wissel3(WISSEL_RECHT);
	usleep(WISSEL_SLEEP);
	wissel4(WISSEL_RECHT);
	usleep(WISSEL_SLEEP);
	wissel5(WISSEL_RECHT);
	usleep(WISSEL_SLEEP);
	wissel6(WISSEL_RECHT);
	usleep(WISSEL_SLEEP);
	wissel7(WISSEL_RECHT);
	usleep(WISSEL_SLEEP);
	wissel8(WISSEL_RECHT);
	usleep(WISSEL_SLEEP);
	wissel9(WISSEL_RECHT);
	
        sleep(2);
	
	printf("checking decouplers!\n");
	ontkoppelaar0();
	sleep(2);
        ontkoppelaar1();
	sleep(2);
        ontkoppelaar2();
	sleep(2);
        ontkoppelaar3();
        
	sleep(2);
	printf("checking train directions!");
	printf("please ensure that the trains ride in the correct direction!");
	locomotief0(LOCOMOTIEF_VOORUIT);
	locomotief1(LOCOMOTIEF_VOORUIT);
	locomotief0(LOCOMOTIEF_LANGZAAM);
	locomotief1(LOCOMOTIEF_LANGZAAM);
	sleep(SEC1*5);
	locomotief0(LOCOMOTIEF_STOP);
	locomotief1(LOCOMOTIEF_STOP);
        locomotief0(LOCOMOTIEF_ACHTERUIT);
        locomotief1(LOCOMOTIEF_ACHTERUIT);
        locomotief0(LOCOMOTIEF_LANGZAAM);
        locomotief1(LOCOMOTIEF_LANGZAAM);
        sleep(5);
        locomotief0(LOCOMOTIEF_STOP);
        locomotief1(LOCOMOTIEF_STOP);
	
}

