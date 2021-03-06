#ifndef SEQUENCE_H
#define SEQUENCE_H
#include <Arduino.h>
#include "Hsv.h"
#include "Quantize.h"

#define MIN_TEMPO 20
#define MAX_TEMPO 300

#define MIDI_MIN 0
#define MIDI_MAX 127

#define GATE_MAX 99
#define GATE_MIN 5

struct Step
{
    uint8_t midiNote;
    bool gate;
    uint8_t gateLength;
    Step();
    Hsv getBaseColor() { return SeqColors::pitchColors[midiNote % 12]; }
};

struct Track
{
    bool gateHigh = false;
    Step steps[16];
    Quantize::TrackQuantizer quant;
    int getNote(uint8_t idx);
    //Returns the number of empty steps following this index OR -1 if this step is off
    int getLength(uint8_t idx);
    //Returns the index of the last step before or at idx that had a note (or -1 if the track is empty)
    int lastTriggeredFrom(uint8_t idx);
    unsigned long lastTriggeredAt = 1;
};

class Sequence
{
public:
    Track tracks[4];
    uint16_t tempo = 120;
    unsigned long periodMicros;
    unsigned long microsIntoCycle;
    uint8_t currentTrack;
    uint8_t currentStep;
    uint8_t selected;
    void setTempo(int newTempo);
    Sequence();
    void setRing(Adafruit_NeoPixel* pixels);
    void setTrackLeds(Adafruit_NeoPixel* pixels, bool quantMode=false);
    unsigned long lastMicros = 0;
    //Rotary encoder handlers
    void shiftSelected(bool dir);
    void shiftTempo(bool dir);
    void shiftNote(bool dir);
    void shiftTrack(bool dir);
    void shiftGateLength(bool dir);
    void shiftQuantizeMode(bool dir);
    void shiftQuantRoot(bool dir);
private:
    Hsv getRingPixelColor(uint8_t step, uint8_t trk);
    Hsv getTrackPixelColor(uint8_t trk, bool quantMode=false);
};
#endif