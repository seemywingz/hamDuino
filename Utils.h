#ifndef UTILS_H
#define UTILS_H
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <LittleFS.h>
#include <WiFiClient.h>

#include "Base64.h"

void runAtInterval(void (*functionToRun)(), unsigned long& lastIntervalRun,
                   unsigned int interval) {
  unsigned long currentRunTime = millis();

  if (currentRunTime - lastIntervalRun >= interval) {
    lastIntervalRun = currentRunTime;
    functionToRun();
  }
}

String makeHTTPSRequest(const String& method, const String& url,
                        const String& payload = "",
                        const String& contentType = "") {
  WiFiClientSecure client;  // Using WiFiClientSecure for HTTPS
  client.setInsecure();     // Disable certificate verification (not recommended
                            // for production)
  HTTPClient http;
  http.begin(client, url);  // Start the client with URL

  // Adding headers if it's a POST request with content
  if (method == "POST" && !payload.isEmpty() && !contentType.isEmpty()) {
    http.addHeader("Content-Type", contentType);
  }

  // Make the HTTP request according to the method
  int httpCode = (method == "POST") ? http.POST(payload) : http.GET();

  // Process the HTTP response
  String response;
  if (httpCode > 0) {  // Check is there's a response
    if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
      response = http.getString();  // Get the response payload
    }
  } else {
    response = "Error on HTTP request: " +
               http.errorToString(httpCode);  // Handle errors
  }

  http.end();  // End the connection
  return response;
}

String formatTimeForSpeech(const String& dateTime) {
  // Example dateTime format: "2021-09-07T17:23:45.123456-07:00"
  int hour = dateTime.substring(11, 13).toInt();    // Extract hours
  int minute = dateTime.substring(14, 16).toInt();  // Extract minutes

  String ampm = "AM";
  if (hour >= 12) {
    ampm = "PM";
    if (hour > 12) hour -= 12;
  }
  if (hour == 0) hour = 12;

  String timeForSpeech = String(hour) + " " + (minute < 10 ? "oh " : "") +
                         String(minute) + " " + ampm;
  return timeForSpeech;
}

String getCurrentTime() {
  String currentTime = "";
  // Fetch the current time
  String url = "https://worldtimeapi.org/api/ip";
  String response = makeHTTPSRequest("GET", url);
  DynamicJsonDocument doc(1024);
  auto error = deserializeJson(doc, response);
  if (error) {
    return error.c_str();
  } else {
    if (doc.containsKey("datetime")) {
      String dateTime = doc["datetime"].as<String>();
      String timeSpeech = formatTimeForSpeech(dateTime);
      return "The Current time is " + timeSpeech;
    } else {
      return "Key 'datetime' not found in JSON response";
    }
  }
}
bool saveDecodedAudio(String base64Audio, const char* filePath) {
  File file = LittleFS.open(filePath, "w");
  if (!file) {
    Serial.println("Failed to open file for writing");
    return false;
  }

  // Decode base64 string to binary and write to file
  int inputLength = base64Audio.length();
  unsigned char* decodedBytes =
      new unsigned char[(inputLength * 3) /
                        4];  // Allocate buffer for decoded bytes

  int bytesDecoded =
      base64_decode(base64Audio.c_str(), decodedBytes, (inputLength * 3) / 4);

  file.write(decodedBytes, bytesDecoded);

  delete[] decodedBytes;  // Free up the buffer
  file.close();
  return true;
}
#endif UTILS_H