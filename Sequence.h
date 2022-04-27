#ifndef SEQUENCE_H
#define SEQUENCE_H
#include <Arduino.h>
#include "Hsv.h"

struct Step
{
    int midiNote;
    bool gate;
    int gateLength;
    Step();
    Hsv getBaseColor() { return SeqColors::pitchColors[midiNote % 12]; }
};

struct Track
{
    bool gateHigh = false;
    Step steps[16];
};

class Sequence
{
public:
    Track tracks[4];
    int tempo = 120;
    unsigned long periodMicros;
    unsigned long microsIntoCycle;
    int currentTrack;
    int currentStep;
    int selected;
    void setTempo(int newTempo);
    Sequence();
    void setRing(Adafruit_NeoPixel* pixels);
    void checkAdvance();
private:
    Hsv getPixelColor(int step, int trk);
    unsigned long lastMicros = 0;
};



#endif