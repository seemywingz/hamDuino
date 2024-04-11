#include <ESPWiFi.h>

// WeMos Pin Config:

// Web Server Config
const String webServerName = "HamDuino";
ESPWiFi wifi = ESPWiFi("HamDuino", "abcd1234");

void setup() {
  Serial.begin(115200);
  while (!Serial) {
  };
  Serial.print("HamDuino starting up...");
  randomSeed(analogRead(A0));
  initializeWebServer();
}

void loop() {
  wifi.handleClient();
  delay(1000);
}

void initializeWebServer() {
  // wifi.setConnectSubroutine([]() { testMatrix(&matrix, LEDWidth, LEDHeight);
  // });
  wifi.enableMDNS(webServerName);
  wifi.start();
}
