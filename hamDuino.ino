#include <AudioGeneratorWAV.h>
#include <AudioFileSourceLittleFS.h>
#include <AudioOutputI2S.h>
#include <ESPWiFi.h>
#include <IOPin.h>

// WeMos Pin Config:
// Radio PTT: D6 (12)
// Audio DIN: RX (03)
// Audio BCK: D8 (15)
// Audio LCK: D4 (02)

// Pins
IOPin ptt = IOPin(12);

// Web Server
const String webServerName = "HamDuino";
ESPWiFi wifi = ESPWiFi("HamDuino", "abcd1234");

// Audio
AudioFileSourceLittleFS *file;
AudioGeneratorWAV *wav;
AudioOutputI2S *dac;

void setup() {
  initializeSerial();
  initializeWebServer();
  initializeAudio();
}

void loop() {
  wifi.handleClient();
  if (wav->isRunning()) {
    if (!wav->loop()) {
      wav->stop();
      ptt.off();
      delay(1000);
    }
  } else {
    playAudio("/wsce496.wav");
  }
}

void playAudio(const char* filename) {
  if (file != nullptr) {
    delete file;
  }
  file = new AudioFileSourceLittleFS(filename);
  ptt.on();
  if (!wav->begin(file, dac)) {
    Serial.println("Failed to begin WAV playback");
    return;
  }
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
  file = new AudioFileSourceLittleFS("/wsce496.wav");
  dac = new AudioOutputI2S();
  dac->SetPinout(15, 2, 3); // BCLK, LRC, DOUT
  wav = new AudioGeneratorWAV();
  // wav->begin(file, dac);
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
