#include <Arduino.h>
#line 1 "/Users/hayden/Desktop/Electronics/Code/CircleSixteen/CircleSixteen.ino"
#include <Adafruit_NeoPixel.h>
#include <MCP48xx.h>
#include <arduino-timer.h>
#include <timer.h>
#include <BfButton.h>
#include <BfButtonManager.h>
#include <RotaryEncoder.h>

#define NUM_STEPS 16
#define NUM_TRACKS 4

#define DAC1_PIN 0
#define DAC2_PIN 1

#define RING 4
#define DCLK 2
#define DDATA 3
#define DSW 5
#define CCLK 9
#define CDATA 10
#define CSW 11
#define BCLK 6
#define BDATA 7
#define BSW 8

#define HALFSTEP_MV 42.626f

// Set up buttons
BfButton dBtn(BfButton::STANDALONE_DIGITAL, DSW, false, HIGH);
BfButton bBtn(BfButton::STANDALONE_DIGITAL, BSW, false, HIGH);
BfButton cBtn(BfButton::STANDALONE_DIGITAL, CSW, false, HIGH);

//Set up DACs
MCP4822 dac1(DAC1_PIN);
MCP4822 dac2(DAC2_PIN);

// Set up the ring
Adafruit_NeoPixel ring(16, RING, NEO_GRB + NEO_KHZ800);

//===========================
// Sequencing logic objects

struct Step
{
    bool gate;      // whether the step has a note
    int midiNum;    // midi note #
    int gateLength; // 0-99 as percentage of step length
    Step()
    {
        gate = false;
        midiNum = 69;
        gateLength = 80;
    }
};

struct Track
{
    Step steps[16];
    bool gateHigh = false;
};

struct Sequence
{
    Track tracks[4];
};
//=========VARIABLES=========================
auto timer = timer_create_default();
RotaryEncoder *encD = nullptr;
RotaryEncoder *encC = nullptr;
RotaryEncoder *encB = nullptr;
int currentStep = 1;
int selected = 1;
int ms = 0;
int tempo = 120;
int periodMs = 250;
int currentTrack = 0;
bool isPlaying = false;
Sequence seq;
//=======Color stuff================================

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

const Hsv hsvColors[] = {cHsv, csHsv, dHsv, dsHsv, eHsv, fHsv, fsHsv, gHsv, gsHsv, aHsv, asHsv, bHsv};

// note will be 0 - 128
#line 105 "/Users/hayden/Desktop/Electronics/Code/CircleSixteen/CircleSixteen.ino"
Hsv forMidiNote(int note);
#line 109 "/Users/hayden/Desktop/Electronics/Code/CircleSixteen/CircleSixteen.ino"
void setRingPixel(int index, Hsv color);
#line 115 "/Users/hayden/Desktop/Electronics/Code/CircleSixteen/CircleSixteen.ino"
Hsv getCurrentPixelColor(int index);
#line 133 "/Users/hayden/Desktop/Electronics/Code/CircleSixteen/CircleSixteen.ino"
Hsv lerp(float p, Hsv a, Hsv b);
#line 142 "/Users/hayden/Desktop/Electronics/Code/CircleSixteen/CircleSixteen.ino"
uint32_t asRgb(Hsv input);
#line 150 "/Users/hayden/Desktop/Electronics/Code/CircleSixteen/CircleSixteen.ino"
void setRingPixels();
#line 161 "/Users/hayden/Desktop/Electronics/Code/CircleSixteen/CircleSixteen.ino"
void setTempo(int newTempo);
#line 171 "/Users/hayden/Desktop/Electronics/Code/CircleSixteen/CircleSixteen.ino"
void checkPositionD();
#line 176 "/Users/hayden/Desktop/Electronics/Code/CircleSixteen/CircleSixteen.ino"
void checkPositionC();
#line 181 "/Users/hayden/Desktop/Electronics/Code/CircleSixteen/CircleSixteen.ino"
void checkPositionB();
#line 187 "/Users/hayden/Desktop/Electronics/Code/CircleSixteen/CircleSixteen.ino"
bool checkAdvance(void *);
#line 199 "/Users/hayden/Desktop/Electronics/Code/CircleSixteen/CircleSixteen.ino"
void advance();
#line 206 "/Users/hayden/Desktop/Electronics/Code/CircleSixteen/CircleSixteen.ino"
void checkEncD();
#line 226 "/Users/hayden/Desktop/Electronics/Code/CircleSixteen/CircleSixteen.ino"
void checkEncC();
#line 243 "/Users/hayden/Desktop/Electronics/Code/CircleSixteen/CircleSixteen.ino"
void checkEncB();
#line 267 "/Users/hayden/Desktop/Electronics/Code/CircleSixteen/CircleSixteen.ino"
void dSwitchPress(BfButton *btn, BfButton::press_pattern_t pattern);
#line 279 "/Users/hayden/Desktop/Electronics/Code/CircleSixteen/CircleSixteen.ino"
void bSwitchPress(BfButton *btn, BfButton::press_pattern_t pattern);
#line 289 "/Users/hayden/Desktop/Electronics/Code/CircleSixteen/CircleSixteen.ino"
void cSwitchPress(BfButton *btn, BfButton::press_pattern_t pattern);
#line 299 "/Users/hayden/Desktop/Electronics/Code/CircleSixteen/CircleSixteen.ino"
void setVoltageForTrack(int trk, uint16_t mV);
#line 320 "/Users/hayden/Desktop/Electronics/Code/CircleSixteen/CircleSixteen.ino"
uint16_t mvForMidiNote(int note);
#line 334 "/Users/hayden/Desktop/Electronics/Code/CircleSixteen/CircleSixteen.ino"
void processTrack(int idx);
#line 355 "/Users/hayden/Desktop/Electronics/Code/CircleSixteen/CircleSixteen.ino"
void setGates();
#line 360 "/Users/hayden/Desktop/Electronics/Code/CircleSixteen/CircleSixteen.ino"
void setPitches();
#line 366 "/Users/hayden/Desktop/Electronics/Code/CircleSixteen/CircleSixteen.ino"
void setup();
#line 417 "/Users/hayden/Desktop/Electronics/Code/CircleSixteen/CircleSixteen.ino"
void loop();
#line 105 "/Users/hayden/Desktop/Electronics/Code/CircleSixteen/CircleSixteen.ino"
Hsv forMidiNote(int note)
{
    return hsvColors[note % 12];
}
void setRingPixel(int index, Hsv color)
{
    ring.setPixelColor(index, asRgb(color));
}

//==============GET PIXEL COLOR
Hsv getCurrentPixelColor(int index)
{
    Hsv color = forMidiNote(seq.tracks[currentTrack].steps[index].midiNum);
    bool gate = seq.tracks[currentTrack].steps[index].gate;
    bool current = index == currentStep;
    bool on = index == selected;
    if (on)
        return {color.h, color.s, 1.0f};
    if (current && gate)
        return lerp(0.5f, color, stepColor);
    else if (current)
        return stepColor;
    else if (gate)
        return color;
    return {0.0f, 0.0f, 0.0f};
}

//basic HSV lerp blending
Hsv lerp(float p, Hsv a, Hsv b)
{
    float h = a.h + ((b.h - a.h) * p);
    float s = a.s + ((b.s - a.s) * p);
    float v = a.v + ((b.v - a.v) * p);
    return {h, s, v};
}

//Convert the HSV value into a 32-bit RGB value
uint32_t asRgb(Hsv input)
{
    return ring.ColorHSV(
        (uint16_t)((input.h / 360.0f) * 65535), 
        (uint8_t)(input.s * 255.0f), 
        (uint8_t)(input.v * 255.0f));
}

void setRingPixels()
{
    ring.clear();
    for (int i = 0; i < 16; i++)
    {
        setRingPixel(i, getCurrentPixelColor(i));
    }
    ring.show();
}
//=================================
// TEMPO CONTROL
void setTempo(int newTempo)
{
    tempo = newTempo;
    float fPeriod = 60.0f / (float)tempo;
    periodMs = fPeriod * 500;
    // Serial.println("Period:");
    // Serial.println(periodMs);
}

//=================Encoder handling=============
void checkPositionD()
{
    encD->tick();
}

void checkPositionC()
{
    encC->tick();
}

void checkPositionB()
{
    encB->tick();
}

// timer callback- checks if it's time to advance to the next step
bool checkAdvance(void *)
{
    ms += 1;
    if (ms >= periodMs)
    {
        ms = 0;
        if (isPlaying)
            advance();
    }
    return true;
}
// move to the next step
void advance()
{
    currentStep = currentStep - 1;
    if (currentStep < 0)
        currentStep = 15;
}

void checkEncD()
{
    static int dPos = 0;
    encD->tick(); // just call tick() to check the state.
    int newPos = encD->getPosition();
    // set selected step
    if (dPos != newPos)
    {
        // Serial.println("D Pos:");
        // Serial.println(newPos);
        auto newOn = selected + (dPos - newPos);
        if (newOn < 0)
            newOn += 16;
        selected = newOn % NUM_STEPS;
        // Serial.println("Current on:");
        // Serial.println(currentOn);
        dPos = newPos;
    }
}

void checkEncC()
{
    static int cPos = 0;
    encC->tick(); // just call tick() to check the state.
    int newPos = encC->getPosition();
    // set selected step
    if (cPos != newPos)
    {
        // Serial.println("C Pos:");
        // Serial.println(newPos);
        int difference = (newPos - cPos) * 3;
        int newTempo = tempo + difference;
        setTempo(newTempo);
        cPos = newPos;
    }
}

void checkEncB()
{
    static int bPos = 0;
    encB->tick();
    int newPos = encB->getPosition();
    if (bPos != newPos)
    {
        auto note = seq.tracks[currentTrack].steps[selected].midiNum;
        auto newNote = note + (bPos - newPos);
        if (note != newNote)
        {
            Serial.println("Note changed to:");
            Serial.println(newNote);
            if (newNote < 0)
                newNote = 0;
            else if (newNote > 127)
                newNote = 127;
            seq.tracks[currentTrack].steps[selected].midiNum = newNote;
        }
        bPos = newPos;
    }
}

//======button handling=============
void dSwitchPress(BfButton *btn, BfButton::press_pattern_t pattern)
{
    // Serial.println("Press type:");
    // Serial.println(pattern);
    if (pattern == BfButton::SINGLE_PRESS)
    {
        // Serial.println("gate is:");
        seq.tracks[currentTrack].steps[selected].gate = !seq.tracks[currentTrack].steps[selected].gate;
        // Serial.println(seq.tracks[currentTrack].steps[currentOn].gate);
    }
}

void bSwitchPress(BfButton *btn, BfButton::press_pattern_t pattern)
{
    if (pattern == BfButton::SINGLE_PRESS)
    {
        isPlaying = !isPlaying;
        Serial.println("Playing:");
        Serial.println(isPlaying);
    }
}

void cSwitchPress(BfButton *btn, BfButton::press_pattern_t pattern)
{
    if (pattern == BfButton::SINGLE_PRESS)
    {
        //TODO: remember what this button is supposed to be for
        
    }
}
//======Output Handling========

void setVoltageForTrack(int trk, uint16_t mV)
{
    switch(trk)
    {
        case 0:
            dac1.setVoltageA(mV);
            break;
        case 1:
            dac1.setVoltageB(mV);
            break;
        case 2:
            dac2.setVoltageA(mV);
            break;
        case 3:
            dac2.setVoltageB(mV);
            break;
        default:
            break;
    }
}
//determines the value to set for the DAC
uint16_t mvForMidiNote(int note)
{
    return (uint16_t)((float)note * HALFSTEP_MV + 0.5);
}
//On each loop, checks if the gate output needs to be high
bool gateOn(int idx, int trk=0)
{
    auto step = seq.tracks[trk].steps[idx];
    if (!step.gate || currentStep != idx)
        return false;
    //check when the gate for this step should end
    auto endMs = (periodMs * (step.gateLength / 80));
    return ms < endMs;
}
void processTrack(int idx)
{
    bool gate = gateOn(currentStep, idx);
    if (gate == seq.tracks[idx].gateHigh)
        return;
    //we know that gate != gateHigh
    if(gate)
    {
        //TODO: set gate output high

        //set the correct v/oct output
        setVoltageForTrack(idx, mvForMidiNote(seq.tracks[idx].steps[currentStep].midiNum));
    }
    else
    {

    }
    seq.tracks[idx].gateHigh = gate;

}

void setGates()
{

}

void setPitches()
{
    // TODO
}

//==========================
void setup()
{
    Serial.begin(9600);
   
    setTempo(tempo);

    //Neo Pixel setup
    ring.begin();
    ring.setBrightness(25);

    //Set up encoders
    encD = new RotaryEncoder(DCLK, DDATA, RotaryEncoder::LatchMode::FOUR3);
    encC = new RotaryEncoder(CCLK, CDATA, RotaryEncoder::LatchMode::FOUR3);
    encB = new RotaryEncoder(BCLK, BDATA, RotaryEncoder::LatchMode::FOUR3);
    attachInterrupt(digitalPinToInterrupt(DCLK), checkPositionD, CHANGE);
    attachInterrupt(digitalPinToInterrupt(DDATA), checkPositionD, CHANGE);
    attachInterrupt(digitalPinToInterrupt(CCLK), checkPositionC, CHANGE);
    attachInterrupt(digitalPinToInterrupt(CDATA), checkPositionC, CHANGE);
    attachInterrupt(digitalPinToInterrupt(BCLK), checkPositionB, CHANGE);
    attachInterrupt(digitalPinToInterrupt(BDATA), checkPositionB, CHANGE);
    
    timer.every(1, checkAdvance);

    //Set up buttons
    dBtn.onPress(dSwitchPress)
        .onDoublePress(dSwitchPress)
        .onPressFor(dSwitchPress, 1000);
    bBtn.onPress(bSwitchPress)
        .onDoublePress(bSwitchPress)     // default timeout
        .onPressFor(bSwitchPress, 1000); // custom timeout for 1 second
    cBtn.onPress(cSwitchPress)
        .onDoublePress(cSwitchPress)     // default timeout
        .onPressFor(cSwitchPress, 1000); // custom timeout for 1 second

    //Set up DACs
    dac1.init();
    dac2.init();

    dac1.turnOnChannelA();
    dac1.turnOnChannelB();

    dac2.turnOnChannelA();
    dac2.turnOnChannelB();

    dac1.setGainA(MCP4822::High);
    dac1.setGainB(MCP4822::High);

    dac2.setGainA(MCP4822::High);
    dac2.setGainB(MCP4822::High);
}

void loop()
{
    timer.tick();
    //poll the inputs
    dBtn.read();
    bBtn.read();
    cBtn.read();
    checkEncD();
    checkEncC();
    checkEncB();
    //set the gate outputs
    setGates();
    //set the v/oct outputs
    setPitches();
    // update the neo pixels
    setRingPixels();
}

