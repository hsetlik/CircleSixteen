# 1 "/Users/hayden/Desktop/Electronics/Code/CircleSixteen/CircleSixteen.ino"
# 2 "/Users/hayden/Desktop/Electronics/Code/CircleSixteen/CircleSixteen.ino" 2
# 3 "/Users/hayden/Desktop/Electronics/Code/CircleSixteen/CircleSixteen.ino" 2
# 4 "/Users/hayden/Desktop/Electronics/Code/CircleSixteen/CircleSixteen.ino" 2
# 5 "/Users/hayden/Desktop/Electronics/Code/CircleSixteen/CircleSixteen.ino" 2
# 6 "/Users/hayden/Desktop/Electronics/Code/CircleSixteen/CircleSixteen.ino" 2
# 7 "/Users/hayden/Desktop/Electronics/Code/CircleSixteen/CircleSixteen.ino" 2
# 8 "/Users/hayden/Desktop/Electronics/Code/CircleSixteen/CircleSixteen.ino" 2
# 32 "/Users/hayden/Desktop/Electronics/Code/CircleSixteen/CircleSixteen.ino"
const uint8_t gatePins[] = {13, 12, A2, A3};
// Set up buttons
BfButton dBtn(BfButton::STANDALONE_DIGITAL, 5, false, 0x1);
BfButton bBtn(BfButton::STANDALONE_DIGITAL, 8, false, 0x1);
BfButton cBtn(BfButton::STANDALONE_DIGITAL, 11, false, 0x1);

//Set up DACs
MCP4822 dac1(0);
MCP4822 dac2(1);

// Set up the ring
Adafruit_NeoPixel ring(16, 4, ((1 << 6) | (1 << 4) | (0 << 2) | (2)) /*|< Transmit as G,R,B*/ + 0x0000 /*|< 800 KHz data transmission*/);
Adafruit_NeoPixel trk(4, A4, ((1 << 6) | (1 << 4) | (0 << 2) | (2)) /*|< Transmit as G,R,B*/ + 0x0000 /*|< 800 KHz data transmission*/);

//===========================
// Sequencing logic objects

struct Step
{
    bool gate; // whether the step has a note
    int midiNum; // midi note #
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
    void clear()
    {
        for(int i = 0; i < 16; ++i)
        {
            steps[i] = Step();
        }
    }
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
RotaryEncoder *encA = nullptr;
int currentStep = 1;
int selected = 1;
unsigned long microsIntoCycle = 0;
int tempo = 120;
unsigned long periodMicros = 250;
int currentTrack = 0;
bool isPlaying = false;
bool tempoMode = false; //ENC c can toggle between controlling tempo and gate length
Sequence seq;
unsigned long lastMicros = 0;
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

const Hsv pitchColors[] = {cHsv, csHsv, dHsv, dsHsv, eHsv, fHsv, fsHsv, gHsv, gsHsv, aHsv, asHsv, bHsv};

const Hsv trk1Hsv = {120.0f, 0.71f, 0.88f};
const Hsv trk2Hsv = {210.0f, 0.71f, 0.88f};
const Hsv trk3Hsv = {300.0f, 0.71f, 0.88f};
const Hsv trk4Hsv = {30.0f, 0.71f, 0.88f};

const Hsv trackColors[] = {trk1Hsv, trk2Hsv, trk3Hsv, trk4Hsv};


// note will be 0 - 128
Hsv forMidiNote(int note)
{
    return pitchColors[note % 12];
}
void setRingPixel(int index, Hsv color)
{
    ring.setPixelColor(index, asRgb(color));
}
void setTrackPixel(int index, Hsv color)
{
    trk.setPixelColor(index, asRgb(color));
}
//==============GET PIXEL COLOR
Hsv getRingPixelColor(int index)
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

Hsv getTrackPixelColor(int index)
{
    auto color = trackColors[index % 4];
    if (currentTrack == index)
        return color;
    if (seq.tracks[index].steps[currentStep].gate)
        return {color.h, color.s, 0.45f};
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
        setRingPixel(i, getRingPixelColor(i));
    }
    ring.show();
}

void setTrackPixels()
{
    trk.clear();
    for(int i = 0; i < 4; ++i)
    {
        setTrackPixel(i, getTrackPixelColor(i));
    }
    trk.show();

}
//=================================
// TEMPO CONTROL
void setTempo(int newTempo)
{
    tempo = newTempo;
    float fPeriod = 60.0f / (float)tempo;
    periodMicros = (unsigned long)(fPeriod * 500000.0f);
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

void checkPositionA()
{
    encB->tick();
}

bool checkAdvance()
{
    auto newMicros = micros();
    microsIntoCycle += (newMicros - lastMicros);
    lastMicros = newMicros;
    if (microsIntoCycle >= periodMicros)
    {
        microsIntoCycle = 0;
        if (isPlaying)
            advance();
    }
    return true;
}
bool updateOutputs(void *)
{
    //update gate/ cv outputs
    for(int i = 0; i < 4; ++i)
    {
        processTrack(i);
    }
    // update the neo pixels
    setRingPixels();
    setTrackPixels();
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
        selected = newOn % 16;
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

void checkEncA()
{
    static int aPos = 0;
    encA->tick(); // just call tick() to check the state.
    int newPos = encA->getPosition();
    // set selected step
    if (aPos != newPos)
    {
        // Serial.println("C Pos:");
        // Serial.println(newPos);
        int difference = newPos - aPos;
        int newTrack = (currentTrack + difference) % 4;
        currentTrack = (newTrack < 0) ? 4 + newTrack : newTrack;
        aPos = newPos;
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

void dSwitchHold(BfButton *btn, BfButton::press_pattern_t pattern)
{
    if (pattern == BfButton::LONG_PRESS)
    {
        seq.tracks[currentTrack].clear();

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
        tempoMode = !tempoMode;
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
    return (uint16_t)((float)note * 42.626f + 0.5);
}
//On each loop, checks if the gate output needs to be high
bool gateOn(int idx, int trk=0)
{
    auto step = seq.tracks[trk].steps[idx];
    if (!step.gate || currentStep != idx)
        return false;
    //check when the gate for this step should end
    auto endMs = (periodMicros * (step.gateLength / 80));
    return microsIntoCycle < endMs;
}
void processTrack(int idx)
{
    bool gate = gateOn(currentStep, idx);
    if (gate == seq.tracks[idx].gateHigh)
        return;
    //we know that gate != gateHigh
    if(gate)
    {
        digitalWrite(gatePins[idx], 1);
        //set the correct v/oct output
        setVoltageForTrack(idx, mvForMidiNote(seq.tracks[idx].steps[currentStep].midiNum));
        Serial.print("Note on on track");
        Serial.print(idx);
        Serial.print(" at ");
        Serial.println(lastMicros);
        dac1.updateDAC();
        dac2.updateDAC();
    }
    else
    {
        digitalWrite(gatePins[idx], 0);
        Serial.print("Note off on track ");
        Serial.print(idx);
        Serial.print(" at ");
        Serial.println(lastMicros);
    }
    seq.tracks[idx].gateHigh = gate;

}
//==========================
void setup()
{
    Serial.begin(9600);

    setTempo(tempo);

    //Neo Pixel setup
    ring.begin();
    ring.setBrightness(25);

    trk.begin();
    trk.setBrightness(25);



    //Set up encoders
    encD = new RotaryEncoder(2, 3, RotaryEncoder::LatchMode::FOUR3);
    encC = new RotaryEncoder(9, 10, RotaryEncoder::LatchMode::FOUR3);
    encB = new RotaryEncoder(6, 7, RotaryEncoder::LatchMode::FOUR3);
    encA = new RotaryEncoder(A0, A1, RotaryEncoder::LatchMode::FOUR3);
    attachInterrupt(((2) == 2 ? 0 : ((2) == 3 ? 1 : -1)), checkPositionD, 1);
    attachInterrupt(((3) == 2 ? 0 : ((3) == 3 ? 1 : -1)), checkPositionD, 1);
    attachInterrupt(((9) == 2 ? 0 : ((9) == 3 ? 1 : -1)), checkPositionC, 1);
    attachInterrupt(((10) == 2 ? 0 : ((10) == 3 ? 1 : -1)), checkPositionC, 1);
    attachInterrupt(((6) == 2 ? 0 : ((6) == 3 ? 1 : -1)), checkPositionB, 1);
    attachInterrupt(((7) == 2 ? 0 : ((7) == 3 ? 1 : -1)), checkPositionB, 1);
    attachInterrupt(((A0) == 2 ? 0 : ((A0) == 3 ? 1 : -1)), checkPositionA, 1);
    attachInterrupt(((A1) == 2 ? 0 : ((A1) == 3 ? 1 : -1)), checkPositionA, 1);

    timer.every(1, updateOutputs);

    //Set up buttons
    dBtn.onPress(dSwitchPress)
        .onDoublePress(dSwitchPress)
        .onPressFor(dSwitchHold, 1000);
    bBtn.onPress(bSwitchPress)
        .onDoublePress(bSwitchPress) // default timeout
        .onPressFor(bSwitchPress, 1000); // custom timeout for 1 second
    cBtn.onPress(cSwitchPress)
        .onDoublePress(cSwitchPress) // default timeout
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

    //set up gate outputs
    for (auto i : gatePins)
    {
        pinMode(i, 0x1);
        digitalWrite(i, 0);
    }
}

void loop()
{
    checkAdvance();
    timer.tick();
    //poll the inputs
    dBtn.read();
    bBtn.read();
    cBtn.read();
    checkEncD();
    checkEncC();
    checkEncB();
    checkEncA();
}
