#line 1 "/Users/hayden/Desktop/Electronics/Code/CircleSixteen/Sequence.cpp"
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
        ring->setPixelColor(i, getRingPixelColor(i, currentTrack).asRgb());
    }
    ring->show();
}

Hsv Sequence::getRingPixelColor(int step, int trk)
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

void Sequence::setTrackLeds(Adafruit_NeoPixel* pixels)
{
    pixels->clear();
    for(int i = 0; i < 4; ++i)
    {
        auto hsv = getTrackPixelColor(i);
        pixels->setPixelColor(i, hsv.asRgb());
    }
    pixels->show();
}

Hsv Sequence::getTrackPixelColor(int trk)
{
    auto base = SeqColors::trackColors[trk % 4];
    if (currentTrack == trk)
        return base;
    else if (tracks[trk].gateHigh)
        return {base.h, base.s, 0.35f};
    return {0.0, 0.0, 0.0};
}

void Sequence::shiftSelected(bool dir)
{
    auto newPos = (dir) ? selected + 1 : selected - 1;
    if (newPos < 0)
        newPos += 16;
    selected = newPos % 16;
}

void Sequence::shiftTempo(bool dir)
{
    tempo = (dir) ? tempo + 5 : tempo - 5;
    if (tempo < MIN_TEMPO)
        tempo = MIN_TEMPO;
    else if (tempo > MAX_TEMPO)
        tempo = MAX_TEMPO;
    setTempo(tempo);
}

void Sequence::shiftNote(bool dir)
{
    int note = tracks[currentTrack].steps[selected].midiNote;
    if (dir && note < MIDI_MAX)
        ++note;
    else if(note >= MIDI_MIN)
        --note;
    tracks[currentTrack].steps[selected].midiNote = note; 
    Serial.println(note);
}

void Sequence::shiftTrack(bool dir)
{
    Serial.print("Current track is: ");
    Serial.print(currentTrack);
    Serial.println();
    Serial.print("Track is: ");
    int newTrack = (dir) ? currentTrack - 1 : currentTrack + 1;
    if (newTrack < 0)
        newTrack += 4;
    Serial.print(newTrack);
    Serial.println();
    Serial.println(((currentTrack - newTrack) < 0)? "increasing" : "decreasing");
    currentTrack = newTrack % 4;   
    Serial.print("New Current track is: ");
    Serial.print(currentTrack);
    Serial.println();
}