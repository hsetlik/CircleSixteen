#ifndef MCPENCODER_H
#define MCPENCODER_H

#include <Arduino.h>
struct Hsv
{
    float h;
    float s;
    float v;
};
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

const Hsv stepColor = {330.0f, 1.0f, 0.6f};

const Hsv pitchColors[] = {cHsv, csHsv, dHsv, dsHsv, eHsv, fHsv, fsHsv, gHsv, gsHsv, aHsv, asHsv, bHsv};

const Hsv trk1Hsv = {120.0f, 0.71f, 0.88f};
const Hsv trk2Hsv = {210.0f, 0.71f, 0.88f};
const Hsv trk3Hsv = {300.0f, 0.71f, 0.88f};
const Hsv trk4Hsv = {30.0f, 0.71f, 0.88f};

const Hsv trackColors[] = {trk1Hsv, trk2Hsv, trk3Hsv, trk4Hsv};




#endif