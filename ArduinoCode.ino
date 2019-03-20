//includes
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <LinkedList.h>

//defines
#define DEBUG 1
//configuration
#define NUMBOARDS 4//27 //specify the amount of boards connected
#define NUMSENS 16
#define PULSETIME 000 //specify the time in ms how long the clock should be high and low

//pins
#define DATA 3
#define CLOCK A1
#define LOAD A2
#define RESET A3

//Data packet info
#define DATA_SRC (0x66)
#define LOCALPORT ((unsigned int)8888)

//global variables:
//static unsigned short dataBuf[NUMBOARDS];
//unsigned long TimerStart;
//unsigned long TimerEnd;
char packetbuff[UDP_TX_PACKET_MAX_SIZE];

//data structure containing sensor updates
struct sensorupdates{
    int16_t sensor;
    int16_t state;
};

//data structure containing the beginning of the message
struct data_struct {
    byte src;
    byte size;
    //unsigned short crc;
} data_pack;

//two databuffers containing raw sensordata
unsigned short dataBuf[NUMBOARDS];
unsigned short compareBuf[NUMBOARDS];

//create a linked list for adding sensor updates
LinkedList<struct sensorupdates> updatelist;

byte mac[] =  {
    0xAB, 0xCD, 0xEF, 0xFE, 0xDC, 0xBA};
IPAddress ip(192,168,1,105);
IPAddress subnet(255,255,255,0);
EthernetServer server(23);

EthernetUDP udp;
bool Connected = false;

// the setup function runs once when you press reset or power the board
void setup() {
    //set pins to input and output
    pinMode(DATA, INPUT);
    pinMode(CLOCK, OUTPUT);
    pinMode(LOAD, OUTPUT);
    pinMode(RESET, OUTPUT);

    //setup ethernet controller
    Ethernet.init(10);
    Ethernet.begin(mac, ip);

#if DEBUG
    //start the serial line for printing data
    Serial.begin(115200);
    while(!Serial){
        delay(1);
    }
    Serial.println("Serial Connection opened!");
#endif

    //check if ethernet hardware is pressent
    if(Ethernet.hardwareStatus() == EthernetNoHardware){
#if DEBUG
        Serial.println("ethernet shield was not found, cant run without hardware...");
#endif
        while(true){
            delay(1000);
        }
    }
    if(Ethernet.linkStatus() == LinkOFF){
        #if DEBUG
        Serial.println("Ethernet cable is not connected.");
        #endif
    }

    //start listening for clients
    udp.begin(LOCALPORT);

#if DEBUG
    Serial.print("Server Address: ");
    Serial.print(Ethernet.localIP());
    Serial.print(LOCALPORT);
#endif

    //setup the data packet
    data_pack.src = DATA_SRC;
    data_pack.size = NUMBOARDS * 2;
}

// the loop function runs over and over again forever
void loop() {
    //wait for a packet
    if(!Connected){
        int packetsize = udp.parsePacket();

        if(packetsize){
#if DEBUG
    Serial.println("incomming message!!");
#endif
        //see if the packet is a propper get request
            udp.read(packetbuff, UDP_TX_PACKET_MAX_SIZE);

#if DEBUG
    Serial.println((String)packetbuff);
#endif

            if((String)packetbuff == "GET plz"){
                Connected = true;
            }
        }
    }else{
        //get the board data, and send it back.
        //delay(1000);
        GetBoards();
        int updateNum = compareData();

        //negeer sensor nummer 37, omdat deze altijd van staat verwisselt.
        for(int listing = 0; listing < updateNum; listing++){
            struct sensorupdates tmpelement = updatelist.get(listing);
            if(tmpelement.sensor == 37){
                updatelist.remove(listing);
                updateNum--;
                break;
            }
        }

        if(updateNum){
            data_pack.size = updateNum;
            PrintData();
        }
        savedata();
    }
}

void GetBoards(){
    //read the data from all the boards
    for(int board = 0; board < NUMBOARDS; board++)
    {
        //reserve a varialbe for the data
        unsigned short data;
        if(board == 0){
            //if this is the first board we are reading, set the crc bytes to 0,
            //and use the special get first function to read the data from the first board
        //    data_pack.crc = 0;
            data = GetFirst();
        }else{
            //else just read the data with the normal readData function
            data = readData();
        }
        //calculate the new crc, and add the data to the packet
        //data_pack.crc ^= data;
        dataBuf[board] = data;
    }
}

unsigned short GetFirst(){
    //reserve a 2 bytes for the data
    unsigned short data = 0;
    //load in the data
    digitalWrite(LOAD, HIGH);
    delayMicroseconds(PULSETIME);
    pulseLine(RESET);
    pulseLine(CLOCK);
    //read the first bit that is comming in
    data = Readbit(data);
    delayMicroseconds(PULSETIME);
    //set the load pin to low again
    digitalWrite(LOAD, LOW);
    //read the rest of the data
    for(int BIT = 1; BIT < 16; BIT++){
        pulseLine(CLOCK);
        data = Readbit(data);
    }
    //Serial.println(data);
    return data;
}

//read and insert the new bit.
unsigned short Readbit(unsigned short data){
    data = data << 1;
    data = data | digitalRead(DATA);
    return data;
}

//give a pulse on the specified line
void pulseLine(uint8_t line){
    digitalWrite(line, HIGH);
    delayMicroseconds(PULSETIME);
    digitalWrite(line, LOW);
    return;
}

//writes to the reset line, to clear the slave device
void resetData(){
    pulseLine(RESET);
    return;
}

//writes to the load line to load data into slave device
void loadData(){
    digitalWrite(LOAD, HIGH);
    delayMicroseconds(PULSETIME);
    pulseLine(CLOCK);
    pulseLine(RESET);
    digitalWrite(LOAD, LOW);
    return;
}

//reads each bit from the line and stores it in the data buffer
unsigned short readData(){
    unsigned short data = 0;
    //shuffle every incomming bit into into the data variables
    for(int Bit = 0; Bit < 16; Bit++){
        //get the incomming bit
        //shuffle the incomming bit into the data
        pulseLine(CLOCK);
        data = data << 1;
        data = data | digitalRead(DATA);  
    }
    return data;
}

int compareData(){
    updatelist.clear();
    struct sensorupdates updateelement;
    //loop through all the boards
    for(int board = 0; board < NUMBOARDS; board++){
        //check what bits have changed
        unsigned short diff = dataBuf[board] ^ compareBuf[board];
        //loop throug every sensor on this board
        for(unsigned char Bit = 0; Bit < NUMSENS; Bit++){
            //get the bit of the sensor we are checking
            unsigned short checkbit = 0x01 << Bit;
            //if this sensor was different from last time
            if(checkbit & diff){
                //create the update
                updateelement.sensor = getsensnumber(board, Bit);
                updateelement.state = getstate(dataBuf[board], checkbit);
                //add it to the list
                updatelist.add(updateelement);
            }
        }
    }
    return updatelist.size();
}

int getsensnumber(int board, unsigned char Bit){
    return (board * NUMSENS + Bit);
}

unsigned char getstate(unsigned short difference, unsigned short checkbit){
    if(difference & checkbit){
        return 1;
    }
    return 0;
}

void savedata(){
    for(int i = 0; i < NUMBOARDS; i++){
        compareBuf[i] = dataBuf[i];
    }
}

void PrintData(){
    //send the data_packet
    udp.beginPacket(udp.remoteIP(), udp.remotePort());
    udp.write((char *)&data_pack, sizeof(data_struct));

    int entries = updatelist.size();
//    struct sensorupdates tmpelement;
    for(int listing = 0; listing < entries; listing++){
        struct sensorupdates tmpelement;
        tmpelement = updatelist.get(listing);
        udp.write((char *)&tmpelement, sizeof(tmpelement));
    }

    udp.endPacket();

#if DEBUG
    Serial.println(updatelist.size());
    for(int listing = 0; listing < entries; listing++){
        struct sensorupdates tmpelement;
        tmpelement = updatelist.get(listing);
        Serial.print("sensor number: ");
        Serial.println(tmpelement.sensor);
        Serial.print("sensor state: ");
        Serial.println(tmpelement.state);
    }
#endif
}