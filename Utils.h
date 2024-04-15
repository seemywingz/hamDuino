#ifndef UTILS_H
#define UTILS_H
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

// Function to make HTTP requests
// Parameters:
// - method: HTTP method (GET, POST, etc.)
// - url: URL to make the request to
// - payload: Request body, if any (for POST requests)
// - contentType: Content type of the request body (for POST requests)
// Returns: HTTP response as a String, or an error message
String makeHttpRequest(const String& method, const String& url,
                       const String& payload = "",
                       const String& contentType = "") {
  WiFiClient client;
  HTTPClient http;
  http.begin(client,
             url);  // Begin by specifying which URL and which client to use

  // Add headers if necessary (for POST)
  if (method == "POST" && payload.length() > 0 && contentType.length() > 0) {
    http.addHeader("Content-Type", contentType);
  }

  // Perform the request
  int httpCode = (method == "POST") ? http.POST(payload) : http.GET();

  // Check the returning code
  String response = "";
  if (httpCode > 0) {  // Check for the returning code
    if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
      response = http.getString();  // Get the request response payload
    }
  } else {
    response = String("Error on HTTP request: ") + http.errorToString(httpCode);
  }

  http.end();  // Close connection
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
  String url = "http://worldtimeapi.org/api/ip";
  String response = makeHttpRequest("GET", url);
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

#endif UTILS_H