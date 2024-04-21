
#include <ESPWiFi.h>

#include "Audio.h"
#include "OpenAI.h"
#include "Utils.h"

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
  handleAudio([]() {
    Serial.println("Responding to RX");
    String whatToSay = openAIChat(
        "Provide a question and answer for the HAM radio technician exam.");
    Serial.println(whatToSay);
    String ttsFile = "/tts.mp3";
    openAI_TTS(whatToSay, ttsFile);
    playAudioFile(ttsFile);
  });

  if (!runOnBoot) {
    runOnBoot = true;
    String ttsFile = "/tts.mp3";
    String hello = openAIChat("Say hello and introduce yourself.");
    Serial.println(hello);
    openAI_TTS(hello, ttsFile);
    playAudioFile(ttsFile);
  }
}

void initializeData() {
  Serial.println("HamDuino starting up...");
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
