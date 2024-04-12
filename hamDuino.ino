
#include <AudioOutputI2SNoDAC.h>
#include <ESP8266SAM.h>
#include <ESPWiFi.h>
#include <IOPin.h>


// WeMos Pin Config:
// PTT Trigger: D6 (12)
// Audio Out: RX 

// Pins
IOPin ptt = IOPin(12);

// Web Server
const String webServerName = "HamDuino";
ESPWiFi wifi = ESPWiFi("HamDuino", "abcd1234");

// Audio
AudioOutputI2SNoDAC *out = NULL;

void setup() {
  initializeSerial();
  initializeAudio();  
  initializeWebServer();
}

void loop() {
  wifi.handleClient();
  ptt.on();
  ESP8266SAM *sam = new ESP8266SAM;
  sam->Say(out, "Can you hear me now?");
  delay(500);
  sam->Say(out, "I can't hear you!");
  delete sam;
  delay(1000);
  ptt.off();
  delay(1000);
}

void initializeAudio() {
  out = new AudioOutputI2SNoDAC();
  out->begin();
}

void initializeSerial () {
  Serial.begin(115200);
  while (!Serial) {};
  Serial.print("HamDuino starting up...");
}

void initializeWebServer() {
  wifi.enableMDNS(webServerName);
  wifi.start();
}
