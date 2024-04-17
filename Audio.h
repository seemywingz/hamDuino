#ifndef AUDIO_H
#define AUDIO_H

// #include <AudioFileSourceID3.h>
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
IOPin spk = IOPin(A0, INPUT);

void initializeAudio() {
  dac = new AudioOutputI2S();
  dac->SetPinout(15, 2, 3);  // BCLK, LRC, DOUT
  wav = new AudioGeneratorWAV();
  ptt.off();
}

void stopAudio() {
  wav->stop();
  // mp3->stop();
  if (fileLFS != nullptr) {
    delete fileLFS;
    fileLFS = nullptr;
  }
}

void playWAVFile(const char *filename) {
  stopAudio();
  fileLFS = new AudioFileSourceLittleFS(filename);
  ptt.on();
  delay(600);
  if (!wav->begin(fileLFS, dac)) {
    Serial.println("Failed to begin WAV playback");
    return;
  }
}

unsigned long lastAudioCheck = 0;
void handleAudio() {
  if (wav->isRunning()) {
    if (!wav->loop()) {
      stopAudio();
    }
  }
  runAtInterval(
      []() {
        int audioLevel = spk.readA();
        // Serial.println("RX Audio Level: " + String(audioLevel));
        if (audioLevel > 1000) {
          Serial.println("RX Audio: " + String(audioLevel));
          ptt.on();
        }
        if (!wav->isRunning() && audioLevel < 1000) {
          ptt.off();
        }
      },
      lastAudioCheck, 100);
}

#endif AUDIO_H