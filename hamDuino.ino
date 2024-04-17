
#include <ESPWiFi.h>

#include "Utils.h"
#include "Audio.h"
#include "OpenAI.h"

// WeMos Pin Config:
// RX Radio SPK: A0
// TX Radio PTT: D6 (12)
// TX Audio DIN: RX (03)
// TX Audio BCK: D8 (15)
// TX Audio LCK: D4 (02)

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
  }, lastLoopRun, 60000);

  if (!ranOnce){ 
    Serial.println("Generating TTS...");
    openAI_TTS("Hello, World, this is HamDuino!", "/tts.wav");
    playWAVFile("/tts.wav");
    ranOnce = true;
  }
}

void initializeData() {
  Serial.begin(115200);
  while (!Serial) {};
  Serial.println("HamDuino starting up...");
  if (!LittleFS.begin()) {
        Serial.println("An Error has occurred while mounting LittleFS");
        return;
    }
  readOpenAIKey("/openAI.key");
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
