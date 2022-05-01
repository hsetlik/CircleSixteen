#include "Quantize.h"
//==========QUANTIZER====================================================================
Quantize::TrackQuantizer::TrackQuantizer() :
rootDegree(0),
mode(ScaleMode::Off)
{
    calculateLut();
}

Quantize::TrackQuantizer::TrackQuantizer(uint8_t rootNote, ScaleMode sMode) :
rootDegree(rootNote % 12),
mode(sMode)
{
    calculateLut();
}

//calculates allowed notes- needs to be called every time the root or mode change
void Quantize::TrackQuantizer::calculateLut()
{
    //if no quantizing, LUT value is equal to its index
    if (mode == ScaleMode::Off)
    {
        for (uint8_t i = 0; i < NUM_MIDI_NOTES; ++i)
        {
            noteLut[i] = i;
        }
        return;
    }
    //figure out which notes are premitted for this scale using sieve for each degree
    bool allowed[NUM_MIDI_NOTES] = {false};
    auto degrees = getDegrees(mode);
    for (uint8_t deg = 0; deg < 7; ++deg)
    {
        auto note = rootDegree + degrees[deg];
        while(note < NUM_MIDI_NOTES)
        {
            allowed[note] = true;
            note += 12;
        }
    }
    //assign each LUT value based on allowed notes
    for (auto i = 0; i < NUM_MIDI_NOTES; ++i)
    {
        noteLut[i] = nearestTrueIndex(allowed, i);
    }
}
uint8_t Quantize::TrackQuantizer::processNote(uint8_t note)
{
    return noteLut[note];
}

void Quantize::TrackQuantizer::setMode(ScaleMode m)
{
    mode = m;
    if (mode != ScaleMode::Off)
        calculateLut();
}

void Quantize::TrackQuantizer::setRoot(uint8_t note)
{
    rootDegree = note % 12;
    calculateLut();
}

void Quantize::TrackQuantizer::nextMode()
{
    int val = ((int) mode + 1) % 7;
    mode = (ScaleMode)val;
    setMode((ScaleMode) val);
}

void Quantize::TrackQuantizer::prevMode()
{
    int val = ((int)mode - 1) % 7;
    if(val < 0)
        val += 8;
    setMode((ScaleMode)val);
}

