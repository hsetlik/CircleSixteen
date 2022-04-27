#include "Sequence.h"
Step::Step()
{
    midiNote = 69;
    gate = false;
    gateLength = 80;
}
//========================
Sequence::Sequence()
{
    microsIntoCycle = 0;
    currentTrack = 0;
    currentStep = 0;
    selected = 0;
    setTempo(tempo);
}

void Sequence::setTempo(int newTempo)
{
    tempo = newTempo;
    float fPeriod = 60.0f / (float)tempo;
    periodMicros = (unsigned long)(fPeriod * 500000.0f);
}

void Sequence::setRing(Adafruit_NeoPixel* ring)
{
    ring->clear();
    for(int i = 0; i < 16; ++i)
    {
        ring->setPixelColor(i, getPixelColor(i, currentTrack).asRgb());
    }
    ring->show();
}

Hsv Sequence::getPixelColor(int step, int trk)
{
    if (step == currentStep)
        return SeqColors::stepColor;
    auto s = tracks[trk].steps[step];
    auto color = s.getBaseColor();
    if (step == selected && s.gate)
    {
        return {color.h, color.s, 1.0f};
    }
    else if (s.gate)
    {
        return color;
    }
    else if (step == selected)
    {
        return SeqColors::selectColor;
    }
    return {0.0f, 0.0f, 0.0f};
}


void Sequence::checkAdvance()
{
    auto newMicros = micros();
    microsIntoCycle += (newMicros - lastMicros);
    lastMicros = newMicros;
    if(microsIntoCycle > periodMicros)
    {
        microsIntoCycle -= periodMicros;
        //TODO: advance
    }
}
