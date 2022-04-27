#include <MCP48xx.h>

#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#include "Sequence.h"

#define RING 2
#define TRACK 3
#define GATEA 4
#define GATEB 5
#define GATEC 6
#define GATEC 7
#define DAC1 8
#define DAC2 9


//=================VARIABLES========================
Sequence seq;
//Neo Pixel strips
Adafruit_NeoPixel ring(16, RING, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel trk(4, TRACK, NEO_GRB + NEO_KHZ800);

MCP4822 dac1(DAC1);
MCP4822 dac2(DAC2);
//================EVENT HANDLING====================
void buttonPressed(int idx)
{

}
void moveEncoder(int idx, bool dir)
{

}
void recieveEvent(int num)
{
    Serial.print("recieved message from ");
    bool isEncoder = Wire.read() == 1;
    Serial.print((isEncoder) ? "encoder " : "button ");
    int idx = Wire.read();
    Serial.println(idx);
    bool dir = Wire.read() == 1;
    if(isEncoder)
    {
        moveEncoder(idx, dir);
    }
    else
    {
        buttonPressed(idx);
    }
}
//======================OUTPUT HANDLING==========================
void updateRing()
{

}
void updateTrk()
{

}

//====================== setup / loop ===========================
void setup()
{
    Serial.begin(9600);
	Wire.begin(8);
    Wire.onReceive(recieveEvent);

    trk.begin();
    ring.begin();
}

void loop()
{


	
}


