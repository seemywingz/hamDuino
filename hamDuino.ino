
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

boolean runOnBoot = false;

unsigned long lastLoopRun = 0;

void loop() {
  wifi.handleClient();
  handleAudio();

  if(!runOnBoot) {
    runOnBoot = true;
    String ttsFile = "/tts.wav";
    openAI_TTS("Hello, From WHISKEY SIERRA CHARLIE ECHO 4 9 6 Repeater. I am an AI ham radio operator. Please, ask me anything.", ttsFile);
    playWAVFile(ttsFile.c_str());
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
