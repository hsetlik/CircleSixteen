#ifndef SEQUENCE_H
#define SEQUENCE_H
#include <Arduino.h>
#include "Hsv.h"

#define MIN_TEMPO 20
#define MAX_TEMPO 300

#define MIDI_MIN 0
#define MIDI_MAX 127

#define GATE_MAX 99
#define GATE_MIN 5

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
    void setTrackLeds(Adafruit_NeoPixel* pixels);
    unsigned long lastMicros = 0;
    void shiftSelected(bool dir);
    void shiftTempo(bool dir);
    void shiftNote(bool dir);
    void shiftTrack(bool dir);
    void shiftGateLength(bool dir);
private:
    Hsv getRingPixelColor(int step, int trk);
    Hsv getTrackPixelColor(int trk);
};
#endif