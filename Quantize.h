#ifndef QUANTIZE_H
#define QUANTUZE_H
#include <Arduino.h>

#define NUM_MIDI_NOTES 88

namespace Quantize
{

    enum ScaleMode
    {
        Off=0,
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

    static const uint8_t* MajorModes[] = 
    {
        IonianDegrees, 
        DorianDegrees, 
        PhrygianDegrees, 
        LydianDegrees, 
        MixolydianDegrees, 
        AeolianDegrees, 
        LocrianDegrees
    };

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
        uint8_t processNote(uint8_t note);
        void nextMode();
        void prevMode();
        void shiftRoot(bool dir)
        {
            int newRoot = (dir) ? rootDegree + 1 : rootDegree - 1;
            if (newRoot < 0)
                newRoot += 12;
            rootDegree = (uint8_t)newRoot;
            calculateLut();
        }
        ScaleMode getMode() {return mode; }
        static const uint8_t* getDegrees(ScaleMode mode)
        {
            auto idx = (uint8_t)mode - 1;
            return MajorModes[idx];
        }
        static uint8_t nearestTrueIndex(bool* table, uint8_t idx)
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
    private:
        //updates the list on valid notes in constructor or on settings change
        void calculateLut();
        uint8_t rootDegree;
        ScaleMode mode;
        //10 octaves- one corresponding to each midi note
        uint8_t noteLut[NUM_MIDI_NOTES];
    };   
}
#endif
