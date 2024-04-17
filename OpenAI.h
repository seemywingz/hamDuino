#ifndef OPENAI_H
#define OPENAI_H

#include "Utils.h"

const String openAI_URL = "https://api.openai.com/v1/";
String openAIKey = "";

String readOpenAIKey(String apiKeyFile) {
  openAIKey = readFile(apiKeyFile);
  return openAIKey;
}

void openAI_TTS(String text, String filePath) {
  String url = openAI_URL + "audio/speech";

  // Prepare the JSON payload
  DynamicJsonDocument doc(1024);
  doc["model"] = "tts-1";
  doc["input"] = text;
  doc["voice"] = "nova";
  doc["response_format"] = "wav";
  String payload;
  serializeJson(doc, payload);
  String contentType = "application/json";

  // Configure the secure client and make the request
  WiFiClientSecure client;
  client.setInsecure();  // Disable certificate verification (not recommended
                         // for production)
  HTTPClient http;
  http.begin(client, url);  // Start the client with URL

  // Set headers
  http.addHeader("Content-Type", contentType);
  http.addHeader("Authorization", "Bearer " + openAIKey);

  int httpCode = http.POST(payload);  // Perform the POST request

  if (httpCode == HTTP_CODE_OK) {
    // Open the file for writing in binary mode
    Serial.println("OpenAI TTS request successful, writing to file...");
    File file = LittleFS.open(filePath, "w+");
    if (!file) {
      Serial.println("Failed to open file for writing");
      http.end();  // End the connection
      return;
    }

    // Stream the response into the file
    // This handles the binary data appropriately by treating it as a stream of
    // bytes
    http.writeToStream(&file);
    file.close();  // Make sure to close the file to save the data
    Serial.println("File written successfully");
  } else {
    Serial.print("HTTP POST failed, error: ");
    Serial.println(http.errorToString(httpCode).c_str());
  }

  http.end();  // End the connection
}

#endif OPENAI_H