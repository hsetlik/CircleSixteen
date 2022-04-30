#include <Wire.h>

#include <BfButton.h>
#include <BfButtonManager.h>

// -----
// SimplePollRotator.ino - Example for the RotaryEncoder library.
// This class is implemented for use with the Arduino environment.
//
// Copyright (c) by Matthias Hertel, http://www.mathertel.de
// This work is licensed under a BSD 3-Clause License. See http://www.mathertel.de/License.aspx
// More information on: http://www.mathertel.de/Arduino
// -----
// 18.01.2014 created by Matthias Hertel
// 04.02.2021 conditions and settings added for ESP8266
// -----

// This example checks the state of the rotary encoder in the loop() function.
// The current position and direction is printed on output when changed.

// Hardware setup:
// Attach a rotary encoder with output pins to
// * A2 and A3 on Arduino UNO.
// * D5 and D6 on ESP8266 board (e.g. NodeMCU).
// Swap the pins when direction is detected wrong.
// The common contact should be attached to ground.

#include <Arduino.h>
#include <RotaryEncoder.h>

// Example for Arduino UNO with input signals on pin 2 and 3
#define A1 2
#define A2 3

#define B1 4
#define B2 5

#define C1 6
#define C2 7

#define D1 8
#define D2 9

#define RST A3;


// Setup a RotaryEncoder with 4 steps per latch for the 2 signal input pins:
// RotaryEncoder encoderA(A1, A2, RotaryEncoder::LatchMode::FOUR3);

const unsigned int buttonPins[] = {10, 11, 12, A0};
BfButton aSw(BfButton::STANDALONE_DIGITAL, 10, true, LOW);
BfButton bSw(BfButton::STANDALONE_DIGITAL, 11, true, LOW);
BfButton cSw(BfButton::STANDALONE_DIGITAL, 12, true, LOW);
BfButton dSw(BfButton::STANDALONE_DIGITAL, A0, true, LOW);

BfButton* buttons[] = {&aSw, &bSw, &cSw, &dSw};

// Setup a RotaryEncoder with 2 steps per latch for the 2 signal input pins:
RotaryEncoder encoderA(A1, A2, RotaryEncoder::LatchMode::FOUR3);
RotaryEncoder encoderB(B1, B2, RotaryEncoder::LatchMode::FOUR3);
RotaryEncoder encoderC(C1, C2, RotaryEncoder::LatchMode::FOUR3);
RotaryEncoder encoderD(D1, D2, RotaryEncoder::LatchMode::FOUR3);


RotaryEncoder encoders[] = {encoderA, encoderB, encoderC, encoderD};

void handleA (BfButton* btn, BfButton::press_pattern_t pattern)
{
  Serial.println("Switch A pressed");
  Wire.beginTransmission(8);
  Wire.write(0); //low bc not an encoder
  Wire.write(0); //Write button index;
  Wire.write(0); //low bc direction is not used
  Wire.endTransmission();
  Serial.println("Switch message sent");
}

void handleB (BfButton* btn, BfButton::press_pattern_t pattern)
{
  Serial.println("Switch B pressed");
  Wire.beginTransmission(8);
  Wire.write(0); //low bc not an encoder
  Wire.write(1); //Write button index;
  Wire.write(0);
  Wire.endTransmission();
  Serial.println("Switch message sent");
}

void handleC (BfButton* btn, BfButton::press_pattern_t pattern)
{
  Serial.println("Switch C pressed");
  Wire.beginTransmission(8);
  Wire.write(0); //low bc not an encoder
  Wire.write(2); //Write button index;
  Wire.write(0);
  Wire.endTransmission();
  Serial.println("Switch message sent");
}

void handleD (BfButton* btn, BfButton::press_pattern_t pattern)
{
  Serial.println("Switch D pressed");
  Wire.beginTransmission(8);
  Wire.write(0); //low bc not an encoder
  Wire.write(3); //Write button index;
  Wire.write(0);
  Wire.endTransmission();
  Serial.println("Switch message sent");
}



void setup()
{
  Serial.begin(9600);
  while (! Serial);
  Serial.println("SimplePollRotator example for the RotaryEncoder library.");

  aSw.onPress(handleA)
    .onDoublePress(handleA)
    .onPressFor(handleA, 1000);
 
  bSw.onPress(handleB)
    .onDoublePress(handleB)
    .onPressFor(handleB, 1000);
  
  cSw.onPress(handleC)
    .onDoublePress(handleC)
    .onPressFor(handleC, 1000);
 
  dSw.onPress(handleD)
    .onDoublePress(handleD)
    .onPressFor(handleD, 1000);

  Wire.begin();
 
} // setup()
int posA = 0;
int posB = 0;
int posC = 0;
int posD = 0;

int* positions[] = {&posA, &posB, &posC, &posD};

// Read the current position of the encoder and print out when changed.
void loop()
{
  for(int i = 0; i < 4; ++i)
  {
    buttons[i]->read();
    encoders[i].tick();
    int newPos = encoders[i].getPosition();
    if (newPos != *positions[i])
    {
      Serial.print("Encoder ");
      Serial.print(i);
      Serial.print(" position: ");
      Serial.print(newPos);
      Serial.print(" with direction ");
      Serial.println(newPos >= *positions[i]);
      Wire.beginTransmission(8);
      Serial.println("Transmission started");
      Wire.write(1);
      Serial.println("First byte sent");
      Wire.write(i);
      Wire.write((newPos >= *positions[i]) ? 1 : 0);
      Wire.endTransmission(); 
      Serial.println("I2C message sent");
      *positions[i] = newPos;
    }
  }
} // loop ()

// The End

