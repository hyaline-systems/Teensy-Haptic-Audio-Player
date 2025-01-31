/*

This patch runs audio from the USB input on the teensy to the output of the board.

If it's patched to the haptics driver (as it is per default), all audio should cause the driver to activate.

*/
#include "Adafruit_DRV2605.h"
#include <Audio.h>
#include <SD.h>
#include <SPI.h>
#include <SerialFlash.h>
#include <Wire.h>

// Audio patch
AudioInputUSB usb1;
AudioOutputI2S i2s1;
AudioConnection patchCord1(usb1, 0, i2s1, 0);
AudioConnection patchCord2(usb1, 1, i2s1, 1);

// Haptics driver
Adafruit_DRV2605 drv;

void setupHapticDriver() {
  drv.begin();

  drv.setMode(DRV2605_MODE_AUDIOVIBE);

  // ac coupled input, puts in 0.9V bias
  drv.writeRegister8(DRV2605_REG_CONTROL1, 0x20);

  // analog input
  drv.writeRegister8(DRV2605_REG_CONTROL3, 0xA3);
}

void setup() {
  AudioMemory(12);
  setupHapticDriver();
}

void loop() {
  // Nothing to do here; the audio processing happens in the background
}
