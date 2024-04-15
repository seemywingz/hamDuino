#include <AudioOutputI2SNoDAC.h>
#include <ESP8266SAM.h>
#include <ESPWiFi.h>
#include <IOPin.h>

// WeMos Pin Config:
// Radio PTT: D6 (12)
// Audio DIN: RX (03)
// Audio BCK: D7 (13)
// Audio LCK: D8 (15)

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
  delay(100);
  sam->Say(out, "Hello, World!");
  ptt.off();
  delay(4000);
}


void runAtInterval(void (*functionToRun)(), unsigned int interval) {
  static unsigned long lastRunTime = 0;
  
  unsigned long currentRunTime = millis();
  unsigned int IntervalMills =
      1000 / interval;  // Duration of each frame in milliseconds

  if (currentRunTime - lastRunTime >= IntervalMills) {
    lastRunTime = currentRunTime;
    functionToRun();
  }
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
