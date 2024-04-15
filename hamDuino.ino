#include <AudioFileSourceBuffer.h>
#include <AudioFileSourceHTTPStream.h>
#include <AudioFileSourceICYStream.h>
#include <AudioFileSourceLittleFS.h>
#include <AudioGeneratorWAV.h>
#include <AudioOutputI2S.h>
#include <ESPWiFi.h>
#include <IOPin.h>

#include "Utils.h"

// WeMos Pin Config:
// Radio SPK: A0
// Radio PTT: D6 (12)
// Audio DIN: RX (03)
// Audio BCK: D8 (15)
// Audio LCK: D4 (02)

// Pins
IOPin ptt = IOPin(12);

// Time
static unsigned long lastIntervalRun = 0;

// Web Server
const String webServerName = "HamDuino";
ESPWiFi wifi = ESPWiFi("HamDuino", "abcd1234");

// Audio
AudioFileSourceLittleFS *fileLFS;
AudioFileSourceICYStream *fileStream;
AudioFileSourceBuffer *buff;
AudioGeneratorWAV *wav;
AudioOutputI2S *dac;

void setup() {
  initializeSerial();
  initializeWebServer();
  initializeAudio();
}

void loop() {
  wifi.handleClient();
  handleAudio();
}

void runAtInterval(void (*functionToRun)(), unsigned int interval) {
  unsigned long currentRunTime = millis();
  unsigned int IntervalMills = interval * 1000;  // Interval in milliseconds

  if (currentRunTime - lastIntervalRun >= IntervalMills) {
    lastIntervalRun = currentRunTime;
    functionToRun();
  }
}

void playAudio(const char *filename) {
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

void streamAudio(const char *url) {
  

  ptt.on();
  // Create a new HTTP audio file source for streaming
  fileStream = new AudioFileSourceICYStream(url);
  buff = new AudioFileSourceBuffer(fileStream, 2048);
  if (!wav->begin(buff, dac)) {
    Serial.println("Failed to begin streaming WAV playback");
    return;
  }
}

void handleAudio() {
  if (wav->isRunning()) {
    if (!wav->loop()) {
      stopAudio();
    }
  } else {
    runAtInterval(
        []() {
          playAudio("/wsce496.wav");
          Serial.println("Playing audio...");
          // streamAudio("https://ccrma.stanford.edu/~jos/wav/gtr-nylon22.wav");
          Serial.println(getCurrentTime());
        },
        9);
  }
}

void stopAudio() {
  if (wav->isRunning()) {
    wav->stop();  // Stop any currently playing audio
    ptt.off();
  }
  if (fileStream != nullptr) {
    delete fileStream;  // Clean up any existing file source
    fileStream = nullptr;
  }
  if (fileLFS != nullptr) {
    delete fileLFS;  // Clean up any existing file source
    fileLFS = nullptr;
  }
  if (buff != nullptr) {
    delete buff;  // Clean up any existing buffer
    buff = nullptr;
  }
}

void initializeAudio() {
  dac = new AudioOutputI2S();
  dac->SetPinout(15, 2, 3);  // BCLK, LRC, DOUT
  wav = new AudioGeneratorWAV();
}

void initializeSerial() {
  Serial.begin(115200);
  while (!Serial) {
  };
  Serial.println("HamDuino starting up...");
}

void initializeWebServer() {
  wifi.enableMDNS(webServerName);
  wifi.start();
}
