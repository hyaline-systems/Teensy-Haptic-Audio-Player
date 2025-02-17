/*

This patch runs audio from the USB input on the teensy to the output of the
board.

If it's patched to the haptics driver (as it is per default), all audio should
cause the driver to activate.

*/
#include "Adafruit_DRV2605.h"
#include <Adafruit_NeoPixel.h>
#include <Audio.h>
#include <SD.h>
#include <SPI.h>
#include <SerialFlash.h>
#include <Wire.h>

// NeoPixel object
constexpr auto LED_PIN = 5;
constexpr auto NUM_LEDS = 1;
Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

// Brightness of the LED: 0-255
constexpr auto brightness = 10;

// Audio patch
AudioInputUSB usb1;  // xy=200,69  (must set Tools > USB Type to Audio)
AudioOutputI2S i2s1; // xy=365,94
AudioConnection patchCord1(usb1, 0, i2s1, 0);
AudioConnection patchCord2(usb1, 1, i2s1, 1);

// Haptics driver
Adafruit_DRV2605 drv;

bool setupHapticDriver() {
  auto result = drv.begin();

  drv.setMode(DRV2605_MODE_AUDIOVIBE);

  // ac coupled input, puts in 0.9V bias
  drv.writeRegister8(DRV2605_REG_CONTROL1, 0x20);

  // analog input
  drv.writeRegister8(DRV2605_REG_CONTROL3, 0xA3);

  return result;
}

void setLEDGreen() {
  const auto color = strip.Color(0, 255, 0);
  strip.fill(color, 0, NUM_LEDS);
  strip.show();
}

void setLEDRed() {
  const auto color = strip.Color(255, 0, 0);
  strip.fill(color, 0, NUM_LEDS);
  strip.show();
}

void setup() {
  Serial.begin(115200);
  strip.begin();
  strip.setBrightness(brightness);
  setLEDRed();

  auto hapticOK = setupHapticDriver();
  if (!hapticOK) {
    Serial.println("Haptic driver setup failed");
    while (1) {

      delay(10);
    };
  }

  AudioMemory(10);

  setLEDGreen();

  Serial.println("Setup done");
  // Turn on Internal LED
  // pinMode(LED_BUILTIN, OUTPUT);
  // digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
  // Nothing to do here; the audio processing happens in the background
}
