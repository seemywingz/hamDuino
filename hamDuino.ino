
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

// Timing Constants
unsigned long pttOnTime = 1000;  // PTT is on for 1 second
unsigned long pttOffTime = 4000; // PTT is off for 4 seconds
unsigned long lastToggleTime = 0;
bool pttState = false;  // Current state of PTT pin, false means OFF

void setup() {
  initializeSerial();
  initializeWebServer();
}

void loop() {
  wifi.handleClient();
  togglePTT(pttOnTime, pttOffTime);
}

void togglePTT(long pttOnTime, long pttOffTime) {
  unsigned long currentTime = millis();
   if (pttState && (currentTime - lastToggleTime >= pttOnTime)) {
    // Turn off PTT
    ptt.off();
    pttState = false;
    lastToggleTime = currentTime;  // Reset the toggle time
  } else if (!pttState && (currentTime - lastToggleTime >= pttOffTime)) {
    // Turn on PTT
    ptt.on();
    pttState = true;
    lastToggleTime = currentTime;  // Reset the toggle time
  }
}


void initializeSerial () {
  Serial.begin(115200);
  while (!Serial) {};
  Serial.println("HamDuino starting up...");
}

void initializeWebServer() {
  wifi.enableMDNS(webServerName);
  wifi.start();
}


