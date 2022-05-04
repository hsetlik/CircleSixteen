#include "Sequence.h"
Step::Step()
{
    midiNote = 45;
    gate = false;
    gateLength = 80;
}
//==========================
int Track::getNote(uint8_t idx)
{
    return quant.processNote(steps[idx].midiNote);
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

Hsv Sequence::getRingPixelColor(uint8_t step, uint8_t trk)
{
    if (step == currentStep)
        return SeqColors::stepColor;
    auto s = tracks[trk].steps[step];
    auto color = Hsv::forMidiNote(tracks[trk].quant.processNote(s.midiNote));
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

void Sequence::setTrackLeds(Adafruit_NeoPixel* pixels, bool quantMode)
{
    pixels->clear();
    for(int i = 0; i < 4; ++i)
    {
        auto hsv = getTrackPixelColor(i);
        if (quantMode && 
        i != currentTrack && 
        (tracks[currentTrack].quant.getMode() != Quantize::ScaleMode::Off))
        {
            int idx = (int)tracks[currentTrack].quant.getMode() - 1;
            hsv = SeqColors::modeColors[idx % 7];
            ////Serial.println("bkgnd is:");
            ////Serial.println(hsv.asRgb());
        } else if (quantMode && i == currentTrack)
        {
            hsv = SeqColors::pitchColors[tracks[currentTrack].quant.getRoot()];
        }
        pixels->setPixelColor(3 - i, hsv.asRgb());
    }
    pixels->show();
}

Hsv Sequence::getTrackPixelColor(uint8_t trk, bool quantMode)
{
    auto base = SeqColors::trackColors[trk % 4];
    if (currentTrack == trk)
        return base;
    else if (tracks[trk].gateHigh)
        return {base.h, base.s, 0.35f};
    return {0.0, 0.0, 0.0};
}


//============Encoder callbacks========================

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
    //Serial.println(note);
}

void Sequence::shiftTrack(bool dir)
{
    int newTrack = (dir) ? currentTrack - 1 : currentTrack + 1;
    if (newTrack < 0)
        newTrack += 4;
    currentTrack = newTrack % 4;   
}

void Sequence::shiftGateLength(bool dir)
{
    auto length = tracks[currentTrack].steps[selected].gateLength;
    length = (dir) ? length + 5 : length - 5;
    if (length > GATE_MAX)
        length = GATE_MAX;
    if (length < GATE_MIN)
        length = GATE_MIN;
    tracks[currentTrack].steps[selected].gateLength = length;
}

void Sequence::shiftQuantizeMode(bool dir)
{
    if (dir)
        tracks[currentTrack].quant.nextMode();
    else
        tracks[currentTrack].quant.prevMode();
}

void Sequence::shiftQuantRoot(bool dir)
{
    tracks[currentTrack].quant.shiftRoot(dir);
}