#include <Wire.h>
#include <Adafruit_NeoPixel.h>

#define RING 2
#define TRACK 3
#define GATEA 4
#define GATEB 5
#define GATEC 6
#define GATEC 7


//================EVENT HANDLING====================
void buttonPressed(int idx)
{

}

void moveEncoder(int idx, bool dir)
{

}
//=================================================

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

    }
}


void setup()
{
    Serial.begin(9600);
	Wire.begin(8);
    Wire.onReceive(recieveEvent);
}

void loop()
{


	
}


