
#include <ESPWiFi.h>

// Web Server
ESPWiFi wifi;

bool runAtStart = false;
String ttsFilePath = "/tts.mp3";

void setup() {
  wifi.start();
  wifi.startAudio();
  wifi.pttEnabled = true;
}

void loop() {
  wifi.handleClient();
  wifi.handleAudio([]() {
    Serial.println("Responding to audio transmission");
    // String response = wifi.openAIChat("HAM Radio Facts and Trivia!");
    // wifi.openAI_TTS(response, ttsFilePath);
    // wifi.playMP3(ttsFilePath);
  });
  if (!runAtStart) {
    runAtStart = true;
    String response = wifi.openAIChat("Hello, world!");
    wifi.openAI_TTS(response, ttsFilePath);
    wifi.playMP3(ttsFilePath);
  }
}
