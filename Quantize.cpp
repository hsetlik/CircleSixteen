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

void Quantize::TrackQuantizer::shiftRoot(bool dir)
{
    int newRoot = (dir) ? rootDegree + 1 : rootDegree - 1;
    if (newRoot < 0)
        newRoot += 12;
    rootDegree = (uint8_t)newRoot;
    calculateLut();
}

uint8_t Quantize::TrackQuantizer::nearestTrueIndex(bool* table, uint8_t idx)
{
    if(table[idx])
        return idx;
    auto upper = idx;
    auto lower = idx;
    while (upper < NUM_MIDI_NOTES && lower > 0)
    {
        if (table[upper])
            return upper;
        if (table[lower])
            return lower;
        ++upper;
        --lower;
    }
    return 0;
}
const uint8_t* Quantize::TrackQuantizer::getDegrees(ScaleMode mode)
        {
            auto idx = (uint8_t)mode - 1;
            return MajorModes[idx];
        }

void Quantize::TrackQuantizer::setMode(ScaleMode m)
{
    mode = m;
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

