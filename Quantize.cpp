#include "Quantize.h"

bool Quantize::inKey(uint8_t rootMidi, uint8_t noteMidi, const uint8_t* degrees)
{
        //think of chromatic scale degrees rather than notes
        auto rootDegree = rootMidi % 12;
        auto noteDegree = noteMidi % 12;
        bool found = false;
        for(int i = 0; i < 7; ++i)
        {
            if (rootDegree + degrees[i] == noteDegree)
                return true;
        }
        return false;
}

//==========QUANTIZER====================================================================
Quantize::TrackQuantizer::TrackQuantizer() :
rootDegree(0),
mode(ScaleMode::Off)
{
    setValidNotes();
}

Quantize::TrackQuantizer::TrackQuantizer(uint8_t rootNote, ScaleMode sMode) :
rootDegree(rootNote % 12),
mode(sMode)
{
    setValidNotes();
}

void Quantize::TrackQuantizer::setValidNotes()
{
    //create a table of all midi notes
    bool included[127];
    for(int i = 0; i < 127; ++i)
    {
         included[i] = false;
    }
    int notesAdded = 0;
    //determine whether each is included and add it to the array
    for(int i = 0; i < 7; ++i)
    {
        auto base = rootDegree + MajorModes[(int)mode][i];
        auto note = base;
        while (note < 127 && notesAdded < 70)
        {
            included[note] = true;
            note += 12;
            validNotes[notesAdded] = note;
            notesAdded += 1;
        }
    }
}

uint8_t Quantize::TrackQuantizer::quantizeUp(uint8_t note)
{
    for(int i = 0; i < 70; ++i)
    {
        //bc notes are always sorted we can just return the first greater value
        if (validNotes[i] >= note)
            return validNotes[i];
    }
    return 0;
}


uint8_t Quantize::TrackQuantizer::quantizeDown(uint8_t note)
{
    //same idea as above but descending
    for (int i = 69; i >= 0; --i)
    {
        if(validNotes[i] <= note)
            return validNotes[i];
    }
    return 0;
}

//gets the nearest valid note
uint8_t Quantize::TrackQuantizer::nearestValid(uint8_t note)
{
    auto upper = quantizeUp(note);
    auto lower = quantizeDown(note);
    auto dUpper = abs(upper - note);
    auto dLower = abs(note - lower);
    return (dUpper > dLower) ? dLower : dUpper;
}

uint8_t Quantize::TrackQuantizer::processNote(uint8_t note)
{
    return (mode == ScaleMode::Off) ? note : nearestValid(note);
}

void Quantize::TrackQuantizer::setMode(ScaleMode m)
{
    mode = m;
    setValidNotes();
}

void Quantize::TrackQuantizer::setRoot(uint8_t note)
{
    rootDegree = note % 12;
    setValidNotes();
}

void Quantize::TrackQuantizer::nextMode()
{
    int val = ((int) mode + 1) % 8;
    mode = (ScaleMode)val;
    setMode((ScaleMode) val);
}

void Quantize::TrackQuantizer::prevMode()
{
    int val = ((int)mode - 1) % 8;
    if(val < 0)
        val += 8;
    setMode((ScaleMode)val);
}

