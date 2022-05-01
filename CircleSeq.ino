#include <MCP48xx.h>

#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#include "Sequence.h"

#define RING 3
#define TRACK 2
#define GATEA 4
#define GATEB 7
#define GATEC 6
#define GATED 5
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
            ////Serial.println("Quantize mode is:");
            ////Serial.println(quantizeMode ? "On" : "Off");
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
                seq.shiftTrack(!dir);
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
        seq.microsIntoCycle = 0;
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
    for(int t = 0; t < 4; ++t)
    {
        auto& trk = seq.tracks[t];
        auto& step = trk.steps[seq.currentStep];
        auto endMicros = (long)seq.periodMicros * (float)((float)step.gateLength / 100.f);
        if(trk.gateHigh && step.gate && seq.microsIntoCycle <= endMicros)
            continue;
        if (trk.gateHigh && step.gate && seq.microsIntoCycle > endMicros)
        {
            //turn gate off
            digitalWrite(gatePins[t], LOW);
            trk.gateHigh = false;
        }
        else if (!trk.gateHigh && step.gate && seq.microsIntoCycle <= endMicros)
        {
            //turn gate on
            digitalWrite(gatePins[t], HIGH);
            trk.gateHigh = true;
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
            dac1.setVoltageB(mV);
            dac1.updateDAC();
            break;
        case 2:
            dac1.setVoltageA(mV);
            dac1.updateDAC();
            break;
        case 3:
            dac2.setVoltageB(mV);
            dac2.updateDAC();
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
    //Serial.begin(9600);
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