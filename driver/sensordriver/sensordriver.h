#ifndef _SENSOR_DRIVER_H_
#define _SENSOR_DRIVER_H_

#define IP "192.168.1.105"
#define PORT 8888

#define NUMBOARDS 27
#define NUMSENS 16

struct data_struct {
    unsigned char src;
    unsigned char size;
    unsigned short dataBuf[NUMBOARDS];
    unsigned short crc;
};
char get_data(int sens_id);
struct data_struct* GetSensorData(void);

#endif

