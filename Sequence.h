#ifndef SEQUENCE_H
#define SEQUENCE_H

#include <Arduino.h>


struct Step
{
    bool gate;      // whether the step has a note
    int midiNum;    // midi note #
    int gateLength; // 0-99 as percentage of step length
    Step()
    {
        gate = false;
        midiNum = 69;
        gateLength = 80;
    }
};

struct Track
{
    Step steps[16];
    bool gateHigh = false;
    void clear()
    {
        for(int i = 0; i < 16; ++i)
        {
            steps[i] = Step();
        }
    }
};

struct Sequence
{
    Track tracks[4];
    int currentTrack = 0;
    int currentStep = 0;
};

#endif