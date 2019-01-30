#include "driver/treinbaandriver.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <treinbaan/wissel.h>
#include <treinbaan/ontkoppelaar.h>
#include <treinbaan/locomotief.h>

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


void rangeer(void)
{
	locomotief0(LOCOMOTIEF_VOORUIT);
	locomotief0(LOCOMOTIEF_SNEL);
	locomotief1(LOCOMOTIEF_VOORUIT);
	locomotief1(LOCOMOTIEF_SNEL);
	
	wissel0(WISSEL_RECHT);
	wissel9(WISSEL_RECHT);
	
	wissel1(WISSEL_KROM);
	wissel8(WISSEL_RECHT);
	wissel2(WISSEL_KROM);
	
	fflush(stdin);
	getchar();
	/*wacht tot sensor subsysteem wagon boven ontkoppelaar detecteert */
	/*while(!(L1 boven O2));*/ /*dit wordt concurrent uitgevoerd, dus dit is geen probleem */
	ontkoppelaar2();
	
	/*while(!(L0 boven O1));*/
	ontkoppelaar1();
	
	wissel3(WISSEL_KROM);
	wissel8(WISSEL_KROM);
	wissel5(WISSEL_RECHT);
	wissel0(WISSEL_KROM);
	wissel4(WISSEL_KROM);
	wissel9(WISSEL_KROM);
	wissel1(WISSEL_RECHT);
	wissel8(WISSEL_KROM);
        wissel7(WISSEL_KROM);
	
	
	/*L1 ontkoppel W1, L2 ontkoppel W3*/
	/*while(!(L1 boven 3));							//wacht totdat L1 bij O13b is*/
        fflush(stdin);
        getchar();
	ontkoppelaar0();
	/*while(!(L2 boven 0));							//wacht totdat L2 bij O12a is*/
	ontkoppelaar3(); 
	
	/*alle wagons weer aankoppelen... zie TreinbaanRangeren.png*/
}
