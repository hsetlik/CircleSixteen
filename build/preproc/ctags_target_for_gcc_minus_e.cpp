# 1 "/Users/hayden/Desktop/Electronics/Code/CircleSixteen/CircleSixteen.ino"
# 2 "/Users/hayden/Desktop/Electronics/Code/CircleSixteen/CircleSixteen.ino" 2
# 3 "/Users/hayden/Desktop/Electronics/Code/CircleSixteen/CircleSixteen.ino" 2
# 4 "/Users/hayden/Desktop/Electronics/Code/CircleSixteen/CircleSixteen.ino" 2
# 5 "/Users/hayden/Desktop/Electronics/Code/CircleSixteen/CircleSixteen.ino" 2
# 6 "/Users/hayden/Desktop/Electronics/Code/CircleSixteen/CircleSixteen.ino" 2
# 7 "/Users/hayden/Desktop/Electronics/Code/CircleSixteen/CircleSixteen.ino" 2
# 8 "/Users/hayden/Desktop/Electronics/Code/CircleSixteen/CircleSixteen.ino" 2
# 25 "/Users/hayden/Desktop/Electronics/Code/CircleSixteen/CircleSixteen.ino"
BfButton dBtn(BfButton::STANDALONE_DIGITAL, 5, false, 0x1);
BfButton bBtn(BfButton::STANDALONE_DIGITAL, 8, false, 0x1);

//Set up the ring
Adafruit_NeoPixel ring(16, 4, ((1 << 6) | (1 << 4) | (0 << 2) | (2)) /*|< Transmit as G,R,B*/ + 0x0000 /*|< 800 KHz data transmission*/);


//===========================
// Sequencing logic objects

struct Step
{
  bool gate; //whether the step has a note
  int midiNum; //midi note #
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
};

struct Sequence
{
  Track tracks[4];
};

//note handling stuff


//create the timer
auto timer = timer_create_default();
//=========VARIABLES=========================

RotaryEncoder* encD = nullptr;
RotaryEncoder* encC = nullptr;
RotaryEncoder* encB = nullptr;
int currentStep = 1;
int currentOn = 1;
long ms = 0;
int tempo = 120;
int periodMs = 250;
int currentTrack = 0;
bool isPlaying = false;
Sequence seq;
//=======Color stuff

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

const Hsv hsvColors[] = {cHsv, csHsv, dHsv, dsHsv, eHsv, fHsv, fsHsv, gHsv, gsHsv, aHsv, asHsv, bHsv};
//========Convert between HSV and RGB=========

//note will be 0 - 128
Hsv forMidiNote(int note)
{
  return hsvColors[note % 12];
}
void setRingPixel(int index, Hsv color)
{
  ring.setPixelColor(index, ring.ColorHSV(color.h, color.s, color.v));
}

//==============GET PIXEL COLOR
Hsv getCurrentPixelColor(int index)
{
  Hsv color = forMidiNote(seq.tracks[currentTrack].steps[index].midiNum);
  bool gate = seq.tracks[currentTrack].steps[index].gate;
  bool current = index == currentStep;
  bool on = index == currentOn;
  if (on)
    return {100.0f, 1.0f, 0.71f};
  else if(current && gate)
    return {color.h, color.s, 1.0f};
  else if(current)
    return {359.0f, 1.0f, 0.81f};
  else if (gate)
    return color;
  return {0.0f, 0.0f, 0.0f};
}

void setRingPixels()
{
  ring.clear();
  for(int i = 0; i < 16; i++)
  {
    setRingPixel(i, getCurrentPixelColor(i));
  }
  ring.show();
}
//=================================
//TEMPO CONTROL
void setTempo(int newTempo)
{
  tempo = newTempo;
  float fPeriod = 60.0f / (float) tempo;
  periodMs = fPeriod * 500;
  //Serial.println("Period:");
  //Serial.println(periodMs);
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

//timer callback- checks if it's time to advance to the next step
bool checkAdvance(void *)
{
  ms += 1;
  if(ms >= periodMs)
  {
    ms = 0;
    if (isPlaying)
      advance();
  }
  return true;
}
//move to the next step
void advance()
{
  currentStep = (currentStep + 1 < 16) ? currentStep + 1 : 0;
}

void checkEncD()
{
  static int dPos = 0;
  encD->tick(); // just call tick() to check the state.
  int newPos = encD->getPosition();
  //set selected step
  if (dPos != newPos)
  {
    //Serial.println("D Pos:");
    //Serial.println(newPos);
    auto newOn = currentOn + (dPos - newPos);
    if (newOn < 0)
      newOn += 16;
    currentOn = newOn % 16;
    //Serial.println("Current on:");
    //Serial.println(currentOn);
    dPos = newPos;
  }
}

void checkEncC()
{
  static int cPos = 0;
  encC->tick(); // just call tick() to check the state.
  int newPos = encC->getPosition();
  //set selected step
  if (cPos != newPos)
  {
    //Serial.println("C Pos:");
    //Serial.println(newPos);
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
    auto note = seq.tracks[currentTrack].steps[currentOn].midiNum;
    auto newNote = note + (bPos - newPos);
    if (note != newNote)
    {
      Serial.println("Note changed to:");
      Serial.println(newNote);
      if (newNote < 0)
        newNote = 0;
      else if (newNote > 127)
        newNote = 127;
      seq.tracks[currentTrack].steps[currentOn].midiNum = newNote;
    }
    bPos = newPos;
  }
}

//======button handling=============
void dSwitchPress(BfButton *btn, BfButton::press_pattern_t pattern)
{
  //Serial.println("Press type:");
  //Serial.println(pattern);
  if(pattern == BfButton::SINGLE_PRESS)
  {
    //Serial.println("gate is:");
    seq.tracks[currentTrack].steps[currentOn].gate = !seq.tracks[currentTrack].steps[currentOn].gate;
    //Serial.println(seq.tracks[currentTrack].steps[currentOn].gate);
  }
}

void bSwitchPress(BfButton *btn, BfButton::press_pattern_t pattern)
{
  if(pattern == BfButton::SINGLE_PRESS)
  {
    isPlaying = !isPlaying;
    Serial.println("Playing:");
    Serial.println(isPlaying);
  }
}
//======Output Handling========
void setGates()
{

}

void setPitches()
{
  //TODO
}

//==========================
void setup()
{
  Serial.begin(9600);
  setTempo(tempo);
  ring.begin();
  ring.setBrightness(25);
  encD = new RotaryEncoder(2, 3, RotaryEncoder::LatchMode::FOUR3);
  encC = new RotaryEncoder(9, 10, RotaryEncoder::LatchMode::FOUR3);
  encB = new RotaryEncoder(6, 7, RotaryEncoder::LatchMode::FOUR3);
  attachInterrupt(((2) == 2 ? 0 : ((2) == 3 ? 1 : -1)), checkPositionD, 1);
  attachInterrupt(((3) == 2 ? 0 : ((3) == 3 ? 1 : -1)), checkPositionD, 1);
  attachInterrupt(((9) == 2 ? 0 : ((9) == 3 ? 1 : -1)), checkPositionC, 1);
  attachInterrupt(((10) == 2 ? 0 : ((10) == 3 ? 1 : -1)), checkPositionC, 1);
  attachInterrupt(((6) == 2 ? 0 : ((6) == 3 ? 1 : -1)), checkPositionB, 1);
  attachInterrupt(((7) == 2 ? 0 : ((7) == 3 ? 1 : -1)), checkPositionB, 1);
  timer.every(1, checkAdvance);

  dBtn.onPress(dSwitchPress)
     .onDoublePress(dSwitchPress)
     .onPressFor(dSwitchPress, 1000);

  bBtn.onPress(bSwitchPress)
     .onDoublePress(bSwitchPress) // default timeout
     .onPressFor(bSwitchPress, 1000); // custom timeout for 1 second
}

void loop()
{
  dBtn.read();
  bBtn.read();
  timer.tick();
  checkEncD();
  checkEncC();
  checkEncB();
  //update the neo pixels
  setRingPixels();
}
