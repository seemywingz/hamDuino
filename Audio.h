#ifndef AUDIO_H
#define AUDIO_H

#include <AudioFileSourceLittleFS.h>
#include <AudioGeneratorWAV.h>
#include <AudioOutputI2S.h>
#include <IOPin.h>

#include "OpenAI.h"
#include "Utils.h"

// Audio
AudioFileSourceLittleFS *fileLFS;
AudioGeneratorWAV *wav;
AudioOutputI2S *dac;

// Pins
IOPin ptt = IOPin(12);
IOPin spk = IOPin(A0, INPUT);

bool receivingAudio = false;
bool responded = true;

void initializeAudio() {
  dac = new AudioOutputI2S();
  dac->SetPinout(15, 2, 3);  // BCLK, LRC, DOUT
  wav = new AudioGeneratorWAV();
  ptt.off();
}

void stopAudio() {
  if (wav->isRunning()) {
    if (!wav->loop()) {
      wav->stop();
    }
  } else if (!receivingAudio) {
    ptt.off();
  }
}

void playAudioFile(String filename) {
  stopAudio();

  delete fileLFS;
  fileLFS = new AudioFileSourceLittleFS(filename.c_str());
  ptt.on();
  delay(600);

  String fileExtention = getFileExtension(filename);
  if (fileExtention == "wav") {
    Serial.println("Playing WAV file: " + String(filename));
    if (!wav->begin(fileLFS, dac)) {
      Serial.println("Failed to begin WAV playback");
      return;
    }
  } else {
    Serial.println("Unsupported file format: " + fileExtention);
    return;
  }
}

void respond() {
  String ttsFile = "/tts.wav";
  String whatToSay = openAIChat("tell a Joke.");
  Serial.println(whatToSay);
  openAI_TTS(whatToSay, ttsFile);
  playAudioFile(ttsFile.c_str());
}

unsigned long lastAudioCheck = 0;
void handleAudio() {
  stopAudio();
  runAtInterval(
      []() {
        int audioLevel = spk.readA();
        if (audioLevel > 1000) {
          Serial.println("RX Audio: " + String(audioLevel));
          if (!receivingAudio) {
            receivingAudio = true;
            responded = false;
            // ptt.on();  // Enable push-to-talk
          }
        } else {
          if (receivingAudio) {
            receivingAudio = false;
          }
        }

        if (!receivingAudio && !responded) {
          Serial.println("Responding to audio...");
          respond();
          responded = true;
        }
      },
      lastAudioCheck, 100);
}

#endif AUDIO_H