
#include <ESPWiFi.h>

// Web Server
ESPWiFi wifi;

String ttsFilePath = "/tts.mp3";

void setup() {
  wifi.start();
  wifi.startAudio();
  wifi.pttEnabled = true;
  wifi.audioResponse = [](){
    Serial.println("Responding to Radio Transmission");
    String response = wifi.openAI_Chat("Provide a question and answer from the technician's HAM radio exam.");
    wifi.openAI_TTS(response, ttsFilePath);
    wifi.playMP3(ttsFilePath);
  };
  String response = wifi.openAI_Chat("Hello, world!");
  wifi.openAI_TTS(response, ttsFilePath);
  wifi.playMP3(ttsFilePath);
}

void loop() {
  wifi.handleClient();
  wifi.handleAudio();
}

