#ifndef AUDIO_H
#define AUDIO_H

#include <AudioFileSourceLittleFS.h>
#include <AudioGeneratorWAV.h>
#include <AudioOutputI2S.h>
#include <IOPin.h>

#include "Utils.h"

// Audio
AudioFileSourceLittleFS *fileLFS;
AudioGeneratorWAV *wav;
AudioOutputI2S *dac;

// Pins
IOPin ptt = IOPin(12);

void initializeAudio() {
  dac = new AudioOutputI2S();
  dac->SetPinout(15, 2, 3);  // BCLK, LRC, DOUT
  wav = new AudioGeneratorWAV();
}

void stopAudio() {
  wav->stop();
  ptt.off();
  if (fileLFS != nullptr) {
    delete fileLFS;  // Clean up any existing file source
    fileLFS = nullptr;
  }
}

void playWAVFile(const char *filename) {
  if (fileLFS != nullptr) {
    delete fileLFS;
  }
  fileLFS = new AudioFileSourceLittleFS(filename);
  ptt.on();
  delay(300);
  tone(3, 693, 1000);
  if (!wav->begin(fileLFS, dac)) {
    Serial.println("Failed to begin WAV playback");
    return;
  }
}

void handleAudio() {
  if (wav->isRunning()) {
    if (!wav->loop()) {
      stopAudio();
    }
  }
}

#endif AUDIO_H