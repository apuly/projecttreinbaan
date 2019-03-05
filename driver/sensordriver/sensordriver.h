#ifndef _SENSOR_DRIVER_H_
#define _SENSOR_DRIVER_H_

/*includes*/
#include "linkedlist.h"

/*defines*/

#define IP "192.168.1.105"
#define PORT 8888

#define NUMBOARDS 4
#define NUMSENS 16

#define UDPBUFSIZE (sizeof(struct dataHeader) + (sizeof(struct sensorupdate) * NUMBOARDS))
#define DATAOFFSET (sizeof(struct dataHeader))

/* data structures */

struct dataHeader {
    unsigned char src;
    unsigned char size;
};

struct data_pack {
    struct dataHeader header;
    struct listItem* listhead;
};

/* function prototypes */

/* initialize the sensors driver */
int SensorInit(int* sock);

/* open socket, and send hello message, returns 0 if succeded 1 if failed */
int OpenSensorSock(int* sock);

/* wait for a sensordata message to appear on the socket, returns data_pack structure */
struct data_pack* GetSensorData(int* sock);

/* closes communication with the sensor subsystem */
int CloseSensorSock(int* sock);

/*prints the list to the screen, for debug purposes*/
void printlist(struct listItem* head);

#endif

