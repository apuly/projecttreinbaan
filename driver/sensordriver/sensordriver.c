#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "sensordriver.h"
#include "linkedlist.h"

struct data_pack *parsePacket(char *data);

/* a global variable ewww... */
struct data_pack parsedPackage;

int main(void)
{
    int sock;
    SensorInit(&sock);
    while (1)
    {
        GetSensorData(&sock);

        printlist(parsedPackage.listhead);

        parsedPackage.listhead = clearlist(parsedPackage.listhead);
    }
}

int SensorInit(int *sock)
{
    int status;
    /* set the head of the linked list to NULL */
    parsedPackage.listhead = NULL;

    status = OpenSensorSock(sock);
    return status;
}

/* open socket, and send hello message, returns 0 if succeded 1 if failed */
int OpenSensorSock(int *sock)
{
    /*reserve variables*/
    struct sockaddr_in serv_addr;
    char Request[] = "GET plz";

    /* create the socket */
    *sock = 0;
    if ((*sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creaton error");
        return 1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    /* Convert IPv4 and IPv6 addresses from text to binary form */
    if (inet_pton(AF_INET, IP, &serv_addr.sin_addr) <= 0)
    {
        perror("invallid adress");
        return 2;
    }

    /* connect to the sensor subsystem */
    if (connect(*sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("socket connection error");
        return 3;
    }

    /* after we have connected, send a get request */
    send(*sock, Request, strlen(Request), 0);

    return 0;
}

/* wait for a sensordata message to appear on the socket, returns data_pack structure */
struct data_pack *GetSensorData(int *sock)
{
    /* reserve some variables */
    int valread;
    char udp_buffer[UDPBUFSIZE];

    /* make sure the buffer is clear */
    memset(udp_buffer, 0, UDPBUFSIZE);

    /* wait for data to come in */
    valread = read(*sock, &udp_buffer, UDPBUFSIZE);

    if (valread == 0)
    {
        /*this function is blocking. so if there is no data, the probram will wait here*/
        return 0;
    }
    else
    {
        /* parse the data */
        return parsePacket(udp_buffer);
    }
}

/* closes communication with the sensor subsystem */
int CloseSensorSock(int *sock)
{
    return 0;
}

struct data_pack *parsePacket(char *data)
{
    struct sensorupdate *sensorPointer;
    struct sensorupdate tmpSensor;
    int noUpdates;
    int index = 0;
    /* make sure the list is clear */
    if (NULL != parsedPackage.listhead)
    {
        parsedPackage.listhead = clearlist(parsedPackage.listhead);
    }
    /* get the ammount of updates */
    noUpdates = ((struct dataHeader *)data)->size;
    /* set the list pointer to the start of the lisitems */
    sensorPointer = (struct sensorupdate *)(data + DATAOFFSET);

    /* walk through the databuffer, and create the list */
    for (index = 0; index < noUpdates; index++)
    {
        tmpSensor = sensorPointer[index];
        parsedPackage.listhead = addItem(parsedPackage.listhead, tmpSensor);
    }

    return &parsedPackage;
}

void printlist(struct listItem* head){
    int count;
    int i;
    count = countItems(head);

        for (i = 0; i < count; i++)
        {
            struct sensorupdate TMPSENS = getItem(head, i);
            write(1,&TMPSENS,sizeof(struct sensorupdate));
          /*  write(1,&TMPSENS.state,sizeof(TMPSENS.state));*/
        }
}
