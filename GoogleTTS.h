#ifndef GOOGLE_TTS_H
#define GOOGLE_TTS_H

#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>

const String GOOGLE_API_KEY = "YOUR_API_KEY";

void googleTextToSpeech(const String& text, void (*callBack)()) {
  WiFiClientSecure client;
  client.setInsecure();  // Not recommended for production, but okay for testing

  HTTPClient http;
  http.begin(client,
             "https://texttospeech.googleapis.com/v1/"
             "text:synthesize?key=" +
                 GOOGLE_API_KEY);
  http.addHeader("Content-Type", "application/json");

  String payload =
      "{\"input\":{\"text\":\"" + text +
      "\"},\"voice\":{\"languageCode\":\"en-US\",\"ssmlGender\":\"FEMALE\"},"
      "\"audioConfig\":{\"audioEncoding\":\"LINEAR16\"}}";
  int httpCode = http.POST(payload);

  if (httpCode > 0) {
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      // Here you would handle the audio file payload, e.g., save it to LittleFS
      // or stream directly
      callBack();
    }
  } else {
    Serial.println("Error in HTTP request");
  }
  http.end();
}

#endif GOOGLE_TTS_H