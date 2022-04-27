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
};

namespace SeqColors
{
    //12 tones spaced evenly along the color wheel
    const Hsv cHsv = {0.0f, 1.0f, 0.6f};
    const Hsv csHsv = {30.0f, 1.0f, 0.6f};
    const Hsv dHsv = {60.0f, 1.0f, 0.6f};
    const Hsv dsHsv = {90.0f, 1.0f, 0.6f};
    const Hsv eHsv = {120.0f, 1.0f, 0.6f};
    const Hsv fHsv = {150.0f, 1.0f, 0.6f};
    const Hsv fsHsv = {180.0f, 1.0f, 0.6f};
    const Hsv gHsv = {210.0f, 1.0f, 0.6f};
    const Hsv gsHsv = {240.0f, 1.0f, 0.6f};
    const Hsv aHsv = {270.0f, 1.0f, 0.6f};
    const Hsv asHsv = {300.0f, 1.0f, 0.6f};
    const Hsv bHsv = {330.0f, 1.0f, 0.6f};

    const Hsv pitchColors[] = {cHsv, csHsv, dHsv, dsHsv, eHsv, fHsv, fsHsv, gHsv, gsHsv, aHsv, asHsv, bHsv};

    const Hsv trackColors[] = {dHsv, fHsv, gsHsv, bHsv};

    const Hsv stepColor = {330.0f, 1.0f, 0.6f};
    const Hsv selectColor = {230.0f, 1.0f, 0.6f};
}

#endif