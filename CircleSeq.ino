#include <MCP48xx.h>

#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#include "Sequence.h"

#define RING 2
#define TRACK 3
#define GATEA 4
#define GATEB 5
#define GATEC 6
#define GATED 7
#define DAC1 8
#define DAC2 9

const int gatePins[] = {GATEA, GATEB, GATEC, GATED};


//=================VARIABLES========================
Sequence seq;
bool isPlaying = false;
//Neo Pixel strips
Adafruit_NeoPixel ring(16, RING, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel trk(4, TRACK, NEO_GRB + NEO_KHZ800);

MCP4822 dac1(DAC1);
MCP4822 dac2(DAC2);
//================EVENT HANDLING====================
void buttonPressed(int idx)
{
    switch (idx)
    {
        case 0: //handle play/stop
        {
            isPlaying = !isPlaying;
            break;
        }
        case 1: //handle gate toggling
        {
            seq.tracks[seq.currentTrack].steps[seq.selected].gate = !seq.tracks[seq.currentTrack].steps[seq.selected].gate;
            break;
        }
        case 2:
        {
            //TODO: toggle between length and tempo mode

        }
        case 3:
        {

        }
    }


}
void moveEncoder(int idx, bool dir)
{
    switch (idx)
    {
        case 0:
        {
            seq.shiftSelected(dir);
            break;
        }
        case 1:
        {
            seq.shiftTempo(dir);
            break;
        }
        case 2:
        {
            seq.shiftNote(dir);
            break;
        }
        case 3:
        {
            seq.shiftTrack(dir);
            break;
        }
    }

}
void recieveEvent(int num)
{
    //Serial.print("recieved message from ");
    bool isEncoder = Wire.read() == 1;
    //Serial.print((isEncoder) ? "encoder " : "button ");
    int idx = Wire.read();
    //Serial.println(idx);
    bool dir = Wire.read() == 1;
    if(isEncoder)
    {
        moveEncoder(idx, dir);
        //Serial.print("Direction: ");
        //Serial.println(dir);
    }
    else
    {
        buttonPressed(idx);
    }
}
//======================TIMER HANDLING===========================
void checkAdvance()
{
    auto newMicros = micros();
    seq.microsIntoCycle += (newMicros - seq.lastMicros);
    if (seq.microsIntoCycle >= seq.periodMicros)
    {
        seq.microsIntoCycle = 0;
        if (isPlaying)
            advance();
    }
    seq.lastMicros = newMicros;
}
void advance()
{
    seq.currentStep = (seq.currentStep < 15) ? seq.currentStep + 1 : 0;
}
//======================OUTPUT HANDLING==========================
void updateRing()
{
    seq.setRing(&ring);
}
void updateTrk()
{
    seq.setTrackLeds(&trk);
}
void updateGates()
{
    for (int i = 0; i < 4; ++i)
    {
        auto trk = &seq.tracks[i];
        if (trk->gateHigh)
        {
            //find the ms where gate should end
            auto end = seq.periodMicros * (trk->steps[i].gateLength / 100);
            if (seq.microsIntoCycle > end)
            {
                trk->gateHigh = false;
                digitalWrite(gatePins[i], LOW);
            }
        }
        else if(seq.tracks[i].steps[seq.currentStep].gate)
        {
            trk->gateHigh = true;
            digitalWrite(gatePins[i], HIGH);
        }

    }

}
void updateDACs()
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

    trk.setBrightness(40);
    ring.setBrightness(40);

    for(int i = 0; i < 4; ++i)
    {
        pinMode(gatePins[i], OUTPUT);
    }
}

void loop()
{
    checkAdvance();
    updateRing();
    updateTrk();
    updateGates();
    updateDACs();
}


