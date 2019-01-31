#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <treinbaan/sensor.h>
#include "sensordriver.h"

int find_board(int sens_id);
int find_sens(int sens_id);
unsigned short get_boardval(struct data_struct* data, int board);
char get_sensval(unsigned short board_val, int bit);

char get_data(int sens_id){
    struct data_struct* data;
    int board;
    int bit;
    unsigned short board_val;
    char state;

    /* first, get wich sensor we need data from */
    board = find_board(sens_id);
    bit = find_sens(sens_id);
    /* check if the board and sensor in question exist */
    if(-1 == board){
        printf("board %d does not exist\n", board);
        exit(1);
    }
    if(-1 == bit){
        printf("pin %d does not exist\n", bit);
        exit(1);
    }

    /* then read the data */
    data = GetSensorData();

    /* lastly return the value of the sensor */
    board_val = get_boardval(data, board);
    state = get_sensval(board_val, bit);

    return state;
}

struct data_struct* GetSensorData(void){
    /*reserve variables*/
    int sock, valread; 
    struct sockaddr_in serv_addr; 
    char Request[] = "GET plz"; 
    static struct data_struct incommingData;

    /* create the socket */
    sock = 0;
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
    { 
        printf("\n Socket creation error \n"); 
        exit(-1); 
    } 
   
    memset(&serv_addr, '0', sizeof(serv_addr)); 
   
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(PORT); 
       
    /* Convert IPv4 and IPv6 addresses from text to binary form */
    if(inet_pton(AF_INET, IP, &serv_addr.sin_addr)<=0)  
    { 
        printf("\nInvalid address/ Address not supported \n"); 
        exit(-1); 
    } 
    
    /* connect to the sensor subsystem */
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    { 
        printf("\nConnection Failed \n"); 
        exit(-1); 
    } 

    /* after we have connected, send a get request */
    send(sock , Request , strlen(Request) , 0 ); 
    /*printf("Data request message sent\n"); */
    /* wait for, and then read the incomming datagram containint the sensordata */
    valread = read( sock , &incommingData, sizeof(incommingData)); 
    /*printf("%d bytes read \n", valread);*/
    
    return &incommingData; 
} 

int find_board(int sens_id){
    int board;
    board = sens_id / NUMSENS;
    if(board < NUMBOARDS){
        return board;
    }else{
        return -1;
    }
}

int find_sens(int sens_id){
    int sensor;
    sensor = sens_id % NUMSENS;
    if(sensor < NUMSENS){
        return sensor;
    }else{
        return -1;
    }
}

unsigned short get_boardval(struct data_struct* data, int board){
    return data->dataBuf[board];
}

char get_sensval(unsigned short board_val, int bit){
    /* we need to get the n'th bit */
    unsigned short shiftedval;

    /* shift the bit we have to read to the rightmost possition */
    shiftedval = board_val >> bit;
    /* then and it with 1 to get the state of the shifted value */
    return (char)(shiftedval & 1);
}
