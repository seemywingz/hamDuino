// #include <AudioOutputI2S.h>
#include <AudioOutputI2SNoDAC.h>
#include <ESP8266SAM.h>
#include <ESPWiFi.h>
#include <IOPin.h>

// WeMos Pin Config:

// Web Server Config
const String webServerName = "HamDuino";
ESPWiFi wifi = ESPWiFi("HamDuino", "abcd1234");

// Audio
// AudioOutputI2S *out = NULL;
AudioOutputI2SNoDAC *out = NULL;
ESP8266SAM *sam = new ESP8266SAM;

// Pins
IOPin ptt = IOPin(12);
// IOPin audioOut = IOPin(15);

void setup() {
  Serial.begin(115200);
  while (!Serial) {
  };
  Serial.print("HamDuino starting up...");
  initializeWebServer();
  out = new AudioOutputI2SNoDAC();
  // out = new AudioOutputI2S();
  out->begin();
}

void loop() {
  wifi.handleClient();
  ptt.on();
  delay(500);
  // tone(audioOut.pin(), 698, 1000);
  // sam->Say(out, "Hello, World! My name is SAM.");
  sam->Say(out,
           "Four score and seven years ago our fathers brought forth on this "
           "continent a new nation, conceived in liberty. ");
  sam->Say(out,
           "and dedicated to the "
           "proposition that all men are created equal.");
  sam->Say(out,
           "Now we are engaged in a great civil war, testing whether that "
           "nation, or any nation so conceived and so dedicated, can long "
           "endure.");
  delay(1000);
  ptt.off();
  delay(1000);
}

void initializeWebServer() {
  // wifi.setConnectSubroutine([]() { testMatrix(&matrix, LEDWidth, LEDHeight);
  // });
  wifi.enableMDNS(webServerName);
  wifi.start();
}
