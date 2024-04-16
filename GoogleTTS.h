#ifndef GOOGLE_TTS_H
#define GOOGLE_TTS_H

#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>

#include "Audio.h"
#include "Utils.h"

const String googleAPIKey = "";

void googleTextToSpeech(const String& text, const char* outputPath) {
  WiFiClientSecure client;
  client.setInsecure();  // For testing, not recommended for production

  HTTPClient http;
  String fullURL =
      "https://texttospeech.googleapis.com/v1/text:synthesize?key=" +
      String(googleAPIKey);

  // Ensure proper JSON format and encoding
  DynamicJsonDocument doc(1024);
  doc["input"]["text"] = text;
  doc["voice"]["languageCode"] = "en-US";
  doc["voice"]["ssmlGender"] = "FEMALE";
  doc["audioConfig"]["audioEncoding"] = "MP3";
  String payload;
  serializeJson(doc, payload);

  http.begin(client, fullURL);
  http.addHeader("Content-Type", "application/json");

  Serial.println("Sending POST request to URL: " + fullURL);
  Serial.println("Payload: " + payload);

  int httpCode = http.POST(payload);
  if (httpCode > 0) {
    if (httpCode == HTTP_CODE_OK) {
      WiFiClient* stream = http.getStreamPtr();
      File file = LittleFS.open(outputPath, "w");
      if (!file) {
        Serial.println("Failed to open file for writing");
        return;
      }

      // Write response directly to file in chunks
      byte buffer[1024] = {0};
      while (stream->available()) {
        size_t len = stream->readBytes(buffer, sizeof(buffer));
        if (len > 0) {
          file.write(buffer, len);
        }
      }
      file.close();
      Serial.println("File saved successfully.");
    } else {
      String response = http.getString();
      Serial.println("HTTP Error: " + String(httpCode) + " " +
                     http.errorToString(httpCode));
      Serial.println("Response: " + response);
    }
  } else {
    Serial.println("Failed to send HTTP POST request");
  }

  http.end();
}

#endif GOOGLE_TTS_H