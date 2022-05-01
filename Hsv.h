#ifndef HSV_H
#define HSV_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

struct Hsv
{
    float h;
    float s;
    float v;
    uint32_t asRgb();
    static Hsv forMidiNote(uint8_t note);
};

namespace SeqColors
{
    //12 tones spaced evenly along the color wheel
    const Hsv cHsv = {0.0f, 1.0f, 0.25f};
    const Hsv csHsv = {30.0f, 1.0f, 0.25f};
    const Hsv dHsv = {60.0f, 1.0f, 0.25f};
    const Hsv dsHsv = {90.0f, 1.0f, 0.25f};
    const Hsv eHsv = {120.0f, 1.0f, 0.25f};
    const Hsv fHsv = {150.0f, 1.0f, 0.25f};
    const Hsv fsHsv = {180.0f, 1.0f, 0.25f};
    const Hsv gHsv = {210.0f, 1.0f, 0.25f};
    const Hsv gsHsv = {240.0f, 1.0f, 0.25f};
    const Hsv aHsv = {270.0f, 1.0f, 0.25f};
    const Hsv asHsv = {300.0f, 1.0f, 0.25f};
    const Hsv bHsv = {330.0f, 1.0f, 0.25f};

    const Hsv pitchColors[] = {cHsv, csHsv, dHsv, dsHsv, eHsv, fHsv, fsHsv, gHsv, gsHsv, aHsv, asHsv, bHsv};

    const Hsv trackColors[] = {dHsv, fHsv, gsHsv, bHsv};

    const Hsv stepColor = {330.0f, 1.0f, 0.25f};
    const Hsv selectColor = {230.0f, 1.0f, 0.25f};

    const Hsv ionianHsv = {0.0f, 1.0f, 0.5f};
    const Hsv dorianHsv = {51.429f, 1.0f, 0.5f};
    const Hsv phrygianHsv = {102.858f, 1.0f, 0.5f};
    const Hsv lydianHsv = {154.287f, 1.0f, 0.5f};
    const Hsv mixolydianHsv = {205.716f, 1.0f, 0.5f};
    const Hsv aeolianHsv = {257.145f, 1.0f, 0.5f};
    const Hsv locrianHsv = {308.574f, 1.0f, 0.5f};

    const Hsv modeColors[] = {ionianHsv, dorianHsv, phrygianHsv, lydianHsv, mixolydianHsv, aeolianHsv, locrianHsv};
}

#endif