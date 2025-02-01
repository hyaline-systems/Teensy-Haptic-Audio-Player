/*
This sketch reads the SD card and plays all WAV files on it.

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

constexpr auto audioDirectory = "/audiofiles";

// NeoPixel object
constexpr auto LED_PIN = 5;
constexpr auto NUM_LEDS = 1;
Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

// Audio patch
AudioPlaySdWav playWav; // Add AudioPlaySdWav object
AudioOutputI2S i2s1;    // xy=365,94
AudioConnection patchCord1(playWav, 0, i2s1, 0);
AudioConnection patchCord2(playWav, 1, i2s1, 1);

constexpr auto cs = 10;
constexpr auto mosi = 11;
constexpr auto sck = 13;
constexpr auto miso = 12;

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

void setLEDOrange() {
  const auto color = strip.Color(255, 165, 0);
  strip.fill(color, 0, NUM_LEDS);
  strip.show();
}

void setup() {
  Serial.begin(115200);
  strip.begin();
  setLEDRed();

  Serial.println("Setting up haptic driver...");
  auto hapticOK = setupHapticDriver();
  if (!hapticOK) {
    Serial.println("Haptic driver setup failed");
    while (1) {
      delay(10);
    };
  }

  Serial.println("Allocating audio memory...");
  AudioMemory(10);

  SPI.setMOSI(mosi);
  SPI.setSCK(sck);
  SPI.setMISO(miso);

  Serial.println("Initializing SD card...");
  if (!SD.begin(cs)) {
    Serial.println("SD card initialization failed!");
    while (1) {
      delay(10);
    }
  }
}

void playNextWavFile(File dir) {
  File entry = dir.openNextFile();
  if (!entry) {
    // no more files
    return;
  }
  if (!entry.isDirectory()) {
    // we only care about .WAV files
    String filename = entry.name();
    const auto weird_file =
        filename.startsWith(".") || filename.startsWith("_");
    if (filename.endsWith(".WAV") ||
        ((filename.endsWith(".wav") && !weird_file))) {
      {
        Serial.print("Playing file: ");
        Serial.println(filename);
        playWav.play(filename.c_str());
        delay(5); // wait for library to parse WAV info
        while (playWav.isPlaying()) {
          setLEDOrange();
          delay(100); // Add a small delay to avoid flooding the serial output
        }
      }
    } else {
      Serial.print("Skipping file: ");
      Serial.println(filename);
    }
    entry.close();
  }
}

void loop() {
  setLEDOrange();
  if (!playWav.isPlaying()) {
    File root = SD.open(audioDirectory);
    playNextWavFile(root);
    root.close();
  }
}
