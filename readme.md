# Circle 16

A sixteen step, four track Eurorack step sequencer based on 2 arduino nano microcontrollers.

## Overview

The sequence is displayed on a ring of 16 RGB LEDs where color indicates step pitch. Along the bottom are the gate and 1v/octave control voltage outputs, each track is represented by its own RGB LED. The interface gives control over each step's pitch and gate length as well as a quantize mode where a sequence can be quantized to a specific mode of the major scale.

## Circuit

The first microcontroller is dedicated to reading the four rotary encoders and their integrated push buttons. Upon turning a knob or pressing a button, the first microcontroller sends an I2C message to the second. The second microcontroller handles all the actual sequencing logic as well as driving the LEDs and controlling the DACs for CV outputs. The module runs on a standard Eurorack +/- 12V supply.

## Controls

### Center Knob

The rotary encoder positioned inside the LED ring controls which step is currently selected, and its push button acts as the play/stop toggle.

### Note Knob

The knob directly below the LED ring is used to modify the selected step. Turning the encoder will change the pitch, and the push button toggles the selected step on and off.

### Tempo Knob

The tempo knob's push button toggles the encoder from controlling the tempo to controlling the relative gate length of the selected step. In quantize mode, the tempo knob controls the tonal center of the scale.

### Track Knob

The track knob's push button toggles quantize mode on and off. By default the encoder selects the track currently being edited, and in quantize mode it selects which scale to which the sequence will be quantized.

