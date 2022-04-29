#ifndef QUANTIZE_H
#define QUANTUZE_H
#include <Arduino.h>

namespace Quantize
{
    enum class ScaleMode
    {
        Ionian,
        Dorian,
        Phrygian,
        Lydian,
        Mixolydian,
        Aeolian,
        Locrian
    };
    //Each mode of the major scale as a list of array indeces
    const uint8_t IonianDegrees[7] = {0, 2, 4, 5, 7, 9, 11};
    const uint8_t DorianDegrees[7] = {0, 2, 3, 5, 7, 9, 10};
    const uint8_t PhrygianDegrees[7] = {0, 1, 3, 5, 7, 8, 10};
    const uint8_t LydianDegrees[7] = {0, 2, 4, 6, 7, 9, 11};
    const uint8_t MixolydianDegrees[7] = {0, 2, 4, 5, 7, 9, 10};
    const uint8_t AeolianDegrees[7] = {0, 2, 3, 5, 7, 8, 10};
    const uint8_t LocrianDegrees[7] = {0, 2, 3, 5, 5, 8, 10};

    const uint8_t* MajorModes[] = 
    {
        IonianDegrees, 
        DorianDegrees, 
        PhrygianDegrees, 
        LydianDegrees, 
        MixolydianDegrees, 
        AeolianDegrees, 
        LocrianDegrees
    };
    bool inKey(uint8_t rootMidi, uint8_t noteMidi, const uint8_t* degrees=IonianDegrees);

    //defines the quantize mode for a given track
    struct QuantizeSettings
    {
        uint8_t rootDegree;
        ScaleMode mode;
    };

    class TrackQuantizer
    {
    public:
        TrackQuantizer();
        TrackQuantizer(uint8_t root, ScaleMode mode);
        void setRoot(uint8_t root);
        void setMode(ScaleMode m);
    private:
        uint8_t quantizeUp(uint8_t note);
        uint8_t quantizeDown(uint8_t note);
        uint8_t nearestValid(uint8_t note);
        //updates the list on valid notes in constructor or on settings change
        void setValidNotes();
        uint8_t rootDegree;
        ScaleMode mode;
        //10 octaves
        uint8_t validNotes[70];
    };   
}
#endif
