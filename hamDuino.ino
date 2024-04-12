
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
ESP8266SAM *sam = NULL;

void setup() {
  initializeSerial();
  initializeWebServer();
  initializeAudio();  
}

void loop() {
  wifi.handleClient();
  ptt.on();
  delay(300);
  sam->Say(out, "Whiskey Sierra Charlie Echo 4 9 6");
  delay(300);
  ptt.off();
  delay(3000);
}

void initializeAudio() {
  out = new AudioOutputI2SNoDAC();
  out->begin();
  sam = new ESP8266SAM;
  sam->SetSpeed(54); // Speed, range usually from 0 to 99
  sam->SetPitch(75); // Pitch, range usually from 0 to 99
  sam->SetMouth(128); // Mouth, range usually from 0 to 255
  sam->SetThroat(128); // Throat, range usually from 0 to 255
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
