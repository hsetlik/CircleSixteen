#include "Hsv.h"

uint32_t Hsv::asRgb()
{
    return Adafruit_NeoPixel::ColorHSV(
        (uint16_t)((h / 360.0f) * 65535), 
        (uint8_t)(s * 255.0f), 
        (uint8_t)(v * 255.0f));
}

Hsv Hsv::forMidiNote(uint8_t note)
{
    return SeqColors::pitchColors[note % 12];

}