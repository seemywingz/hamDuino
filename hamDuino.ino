
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
  initializeData();
  initializeWebServer();
  initializeAudio();
}

boolean ranOnce = false;

unsigned long lastLoopRun = 0;

void loop() {
  wifi.handleClient();
  handleAudio();
  runAtInterval([]() {
    Serial.println(getCurrentTime());
    Serial.println("Playing audio...");
    playWAVFile("/wsce496.wav");
  }, lastLoopRun, 9000);
  // if (!ranOnce){ 
    // Serial.println("Generating TTS...");
    // googleTextToSpeech("Hello, world!", "/tts.mp3");
    // ranOnce = true;
  // }
}

void initializeData() {
  Serial.begin(115200);
  while (!Serial) {};
  Serial.println("HamDuino starting up...");
  if (!LittleFS.begin()) {
        Serial.println("An Error has occurred while mounting LittleFS");
        return;
    }
}

void initializeWebServer() {

  // List all files in the LittleFS
    wifi.webServer.on("/files", HTTP_GET, []() {
        String message = "Files on LittleFS:<br>";
        Dir dir = LittleFS.openDir("/");
        while (dir.next()) {
            String fileName = dir.fileName();
            message += "<a href=\"" + fileName + "\">" + fileName + "</a><br>";
        }
        wifi.webServer.send(200, "text/html", message);
    });

  wifi.enableMDNS(webServerName);
  wifi.start();
}
