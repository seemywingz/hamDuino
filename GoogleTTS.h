#ifndef GOOGLE_TTS_H
#define GOOGLE_TTS_H

#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>

#include "Audio.h"
#include "Utils.h"

const String googleAPIKey = "";
const String googleURL = "https://texttospeech.googleapis.com/v1/";

void googleTextToSpeech(const String& text, const char* outputPath) {
  WiFiClientSecure client;
  client.setInsecure();  // Disable certificate verification for simplicity (not
                         // recommended for production)
  HTTPClient http;

  // Google API URL with API Key
  String googleURL =
      "https://texttospeech.googleapis.com/v1/text:synthesize?key=" +
      String(googleAPIKey);

  // Setup JSON payload
  String payload = "{\"input\":{\"text\":\"" + text +
                   "\"},\"voice\":{\"languageCode\":\"en-US\",\"ssmlGender\":"
                   "\"FEMALE\"},\"audioConfig\":{\"audioEncoding\":\"mp3\"}}";

  http.begin(client, googleURL);
  http.addHeader("Content-Type", "application/json");

  int httpCode = http.POST(payload);  // Send the request

  if (httpCode == HTTP_CODE_OK) {
    File file = LittleFS.open(outputPath, "w");
    if (!file) {
      Serial.println("Failed to open file for writing");
      return;
    }

    // Get the response as a stream
    WiFiClient* stream = http.getStreamPtr();

    // Read all the data received and save it to the file
    uint8_t buffer[1024];  // Buffer to hold chunks of data
    while (http.connected() && stream->available()) {
      size_t len = stream->readBytes(buffer, sizeof(buffer));
      if (len > 0) {
        file.write(buffer, len);  // Write to file
      }
    }
    file.close();  // Close the file when done
    Serial.println("Audio content saved successfully to LittleFS");
  } else {
    Serial.println("Error in HTTP request: " + http.errorToString(httpCode));
  }

  http.end();  // End the HTTP connection
}

#endif GOOGLE_TTS_H