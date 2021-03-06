#include <MCP48xx.h>

#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#include "Sequence.h"

#define RING 3
#define TRACK 2
#define GATEA 4
#define GATEB 5
#define GATEC 6
#define GATED 7
#define DAC1 8
#define DAC2 9

//this corresponds to an external amplifier with a gain of 2 to meet volt/octave scale
#define HALFSTEP_MV 42.626f

const uint8_t gatePins[] = {GATEA, GATEB, GATEC, GATED};

//=================VARIABLES========================
Sequence seq;
bool isPlaying = false;
bool tempoMode = true;
//TODO: set up quantize mode
bool quantizeMode = false;
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
            quantizeMode = !quantizeMode;
            break;
        }
        case 1: //handle gate toggling
        {
            seq.tracks[seq.currentTrack].steps[seq.selected].gate = !seq.tracks[seq.currentTrack].steps[seq.selected].gate;
            break;
        }
        case 2:
        {
            tempoMode = !tempoMode;
            break;
        }
        case 3:
        {
            isPlaying = !isPlaying;
            break;
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
            if(quantizeMode)
                seq.shiftQuantRoot(dir);
            else if (tempoMode)
                seq.shiftTempo(dir);
            else
                seq.shiftGateLength(dir);
            break;
        }
        case 2:
        {
            seq.shiftNote(!dir);
            break;
        }
        case 3:
        {
            if (quantizeMode)
                seq.shiftQuantizeMode(dir);
            else
                seq.shiftTrack(dir);
            break;
        }
    }
}
//===========I2C input handling=================================
void recieveEvent(int num)
{
    bool isEncoder = Wire.read() == 1;
    int idx = Wire.read();
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
//======================TIMER HANDLING===========================
void checkAdvance()
{
    auto newMicros = micros();
    seq.microsIntoCycle += (newMicros - seq.lastMicros);
    if (seq.microsIntoCycle >= seq.periodMicros)
    {
        seq.microsIntoCycle -= seq.periodMicros;
        if (isPlaying)
            advance();
    }
    seq.lastMicros = newMicros;
}
void advance()
{
    seq.currentStep = (seq.currentStep > 0) ? seq.currentStep - 1 : 15;
}
//======================OUTPUT HANDLING==========================
void updateRing()
{
    seq.setRing(&ring);
}
void updateTrk()
{
    seq.setTrackLeds(&trk, quantizeMode);
}
void updateGates()
{
    if (!isPlaying)
        return;
    for(uint8_t t = 0; t < 4; ++t)
    {
        //Step 1: figure out if the track needs to be triggered
        Track& trk =  seq.tracks[t];
        auto last = trk.lastTriggeredFrom(seq.currentStep);
        auto now = micros();
        //Do nothing if this track is empty
        if (last == -1)
            continue;
        auto stepStart = now - seq.microsIntoCycle;
        if (last != seq.currentStep)
        {
            auto diff = (int)abs(seq.currentStep - last) * seq.periodMicros;
            stepStart -= diff;
        }
        auto length = (unsigned long)(seq.periodMicros * (float)(trk.steps[last].gateLength / 100.0f));
        auto over = now >= stepStart + length;
        //turn the gate on as needed
        if (last == seq.currentStep && !over)
        {
            trk.lastTriggeredAt = now;
            trk.gateHigh = true;
            digitalWrite(gatePins[t], HIGH);
        }
        else if (trk.gateHigh && over)
        {
            trk.gateHigh = false;
            digitalWrite(gatePins[t], LOW);
        }
    }
}
void setVoltageForTrack(int trk, uint16_t mV)
{
    //Serial.println("Setting DAC voltage");
    //Serial.println(mV);
    switch(trk)
    {
        case 0:
            dac2.setVoltageA(mV);
            dac2.updateDAC();
            break;
        case 1:
            dac2.setVoltageB(mV);
            dac2.updateDAC();
            break;
        case 2:
            dac1.setVoltageA(mV);
            dac1.updateDAC();
            break;
        case 3:
            dac1.setVoltageB(mV);
            dac1.updateDAC();
            break;
        default:
            break;
    }
}

uint16_t mvForMidiNote(int note)
{
    return (uint16_t)((float)note * HALFSTEP_MV);
}

void updateDACs()
{
    for(int i = 0; i < 4; ++i)
    {
        if (seq.tracks[i].gateHigh)
        {
            //Serial.println("DAC update needed on: ");
            //Serial.println(i);
            auto mv = mvForMidiNote(seq.tracks[i].getNote((uint8_t)seq.currentStep));
            setVoltageForTrack(i, mv);
        }
    }
}

//====================== setup / loop ===========================
void setup()
{
    Serial.begin(9600);
	Wire.begin(8);
    Wire.onReceive(recieveEvent);

    trk.begin();
    ring.begin();

    trk.setBrightness(30);
    ring.setBrightness(30);

    for(int i = 0; i < 4; ++i)
    {
        pinMode(gatePins[i], OUTPUT);
        digitalWrite(gatePins[i], LOW);
    }
    dac1.init();
    dac2.init();

    dac1.turnOnChannelA();
    dac1.turnOnChannelB();

    dac2.turnOnChannelA();
    dac2.turnOnChannelB();

    dac1.setGainA(MCP4822::High);
    dac1.setGainB(MCP4822::High);

    dac2.setGainA(MCP4822::High);
    dac2.setGainB(MCP4822::High);
}

void loop()
{
    checkAdvance();
    updateRing();
    updateTrk();
    updateGates();
    updateDACs();
}