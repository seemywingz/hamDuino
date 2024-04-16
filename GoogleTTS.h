#ifndef GOOGLE_TTS_H
#define GOOGLE_TTS_H

#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>

#include "Utils.h"

const String googleAPIKey = "";
const String googleURL = "https://texttospeech.googleapis.com/v1/";

void googleTextToSpeech(const String& text,
                        void (*callBack)(String audioContent)) {
  String response = makeHTTPSRequest(
      "POST", googleURL + "text:synthesize?key=" + googleAPIKey,
      "{\"input\":{\"text\":\"" + text +
          "\"},\"voice\":{\"languageCode\":\"en-US\",\"ssmlGender\":\"FEMALE\"}"
          ",\"audioConfig\":{\"audioEncoding\":\"LINEAR16\"}}",
      "application/json");

  // Parse the response
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, response);
  String audioContent =
      doc["audioContent"];  // Assuming 'audioContent' holds relevant data
  callBack(audioContent);
}

#endif GOOGLE_TTS_H