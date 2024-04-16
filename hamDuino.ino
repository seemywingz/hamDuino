
#include <ESPWiFi.h>

#include "Utils.h"
#include "Audio.h"
#include "GoogleTTS.h"

// WeMos Pin Config:
// Radio SPK: A0
// Radio PTT: D6 (12)
// Audio DIN: RX (03)
// Audio BCK: D8 (15)
// Audio LCK: D4 (02)

// Web Server
const String webServerName = "HamDuino";
ESPWiFi wifi = ESPWiFi("HamDuino", "abcd1234");

void setup() {
  initializeSerial();
  initializeWebServer();
  initializeAudio();
}

void loop() {
  wifi.handleClient();
  handleAudio();
   runAtInterval(
        []() {
          Serial.println("Playing audio...");
          playWAVFile("/wsce496.wav");
          Serial.println(getCurrentTime());
          googleTextToSpeech("Hello, world!", [](String audioContent) {
            Serial.println("Google TTS callback");
            Serial.println(audioContent);
          });
        },
        9);
}

void initializeSerial() {
  Serial.begin(115200);
  while (!Serial) {};
  Serial.println("HamDuino starting up...");
}

void initializeWebServer() {
  wifi.enableMDNS(webServerName);
  wifi.start();
}
