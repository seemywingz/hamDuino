#ifndef AUDIO_H
#define AUDIO_H

#include <AudioFileSourceHTTPStream.h>
#include <AudioFileSourceLittleFS.h>
#include <AudioGeneratorWAV.h>
#include <AudioOutputI2S.h>
#include <IOPin.h>

#include "OpenAI.h"
#include "Utils.h"

// Audio
AudioFileSourceLittleFS *fileLFS;
AudioFileSourceHTTPStream *fileStream;
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

void respond() {
  String ttsFile = "/tts.wav";
  String whatToSay = openAIChat("tell a Joke.");
  Serial.println(whatToSay);
  openAI_TTS(whatToSay, ttsFile);
  playWAVFile(ttsFile.c_str());
  // Serial.println("Speech to Text" + openAI_STT("/speech.wav"));
}

unsigned long lastAudioCheck = 0;
void handleAudio() {
  if (wav->isRunning()) {
    if (!wav->loop()) {
      stopAudio();
    }
  } else if (!wav->isRunning() && !receivingAudio) {
    ptt.off();  // Turn off push-to-talk when audio is not playing
  }

  runAtInterval(
      []() {
        int audioLevel = spk.readA();
        if (audioLevel > 1000) {  // Assuming 1000 is your threshold for
                                  // detecting received audio
          Serial.println("RX Audio: " + String(audioLevel));
          if (!receivingAudio) {  // Only change state if previously not
                                  // receiving audio
            receivingAudio = true;
            responded = false;
            // ptt.on();  // Enable push-to-talk
          }
        } else {
          if (receivingAudio) {  // Only change state if it was previously
                                 // receiving
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