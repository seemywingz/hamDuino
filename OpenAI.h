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
  if (openAIKey == "") {
    Serial.println("OpenAI key not set, please call readOpenAIKey() first");
    return;
  }

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

String openAIChat(String text) {
  if (openAIKey == "") {
    Serial.println("OpenAI key not set, please call readOpenAIKey() first");
    return "";
  }

  String url = openAI_URL + "chat/completions";

  // Prepare the JSON payload
  DynamicJsonDocument doc(1024);
  doc["model"] = "gpt-4-turbo-preview";
  doc["messages"] = JsonArray();
  doc["messages"].add(JsonObject());
  doc["messages"][0]["role"] = "system";
  doc["messages"][0]["content"] =
      "You are an AI ham radio operator."
      "Your call sign is WSCE496 or W S C E 4 9 6."
      "Use the call sign to refer to yourself."
      "End with your call sign, or other radio appropriate etiquette, but mix "
      "it up."
      "Use the NATO phonetic alphabet when reciting letters."
      "You are here to help the user with their questions."
      "You can also tell jokes."
      "Be as funny and punny as possible when telling jokes"
      "try to keep jokes radio related.";
  doc["messages"].add(JsonObject());
  doc["messages"][1]["role"] = "user";
  doc["messages"][1]["content"] = text;
  doc["max_tokens"] = 60;
  String payload;
  serializeJson(doc, payload);
  String contentType = "application/json";

  // Make the request
  String response =
      makeHTTPSRequest("POST", url, openAIKey, contentType, payload);

  DynamicJsonDocument resDoc(1024);
  deserializeJson(resDoc, response);
  if (resDoc["choices"][0]["message"]["content"].is<String>()) {
    return resDoc["choices"][0]["message"]["content"].as<String>();
  } else {
    return "Error parsing response";
  }

  return "Sorry, I didn't understand that. Please try again.";
}

String openAI_STT(const String& filePath) {
  if (openAIKey.isEmpty()) {
    Serial.println("OpenAI key not set, please set your OpenAI API key");
    return "API key not set";
  }

  File audioFile = LittleFS.open(filePath, "r");
  if (!audioFile) {
    Serial.println("Failed to open file for reading");
    return "Failed to open file";
  }

  HTTPClient http;
  WiFiClientSecure client;
  client.setInsecure();  // Not recommended for production, manage SSL properly

  String url = "https://api.openai.com/v1/audio/transcriptions";
  http.begin(client, url);  // Start the client with URL

  String boundary = "----WebKitFormBoundary7MA4YWxkTrZu0gW";
  String contentType = "multipart/form-data; boundary=" + boundary;
  http.addHeader("Content-Type", contentType);
  http.addHeader("Authorization", "Bearer " + openAIKey);

  String body;
  String modelPart =
      "--" + boundary +
      "\r\nContent-Disposition: form-data; name=\"model\"\r\n\r\nwhisper-1\r\n";
  String header = "--" + boundary +
                  "\r\nContent-Disposition: form-data; name=\"file\"; "
                  "filename=\"audio.wav\"\r\nContent-Type: audio/wav\r\n\r\n";
  String footer = "\r\n--" + boundary + "--";

  http.addHeader("Content-Length",
                 String(header.length() + audioFile.size() +
                        modelPart.length() + footer.length()));

  // Start the POST request
  http.POST("");

  // Send header and file content
  http.getStream().print(header);
  byte buffer[512];
  size_t bytesRead = 0;
  while ((bytesRead = audioFile.read(buffer, sizeof(buffer))) > 0) {
    http.getStream().write(buffer, bytesRead);
  }

  // Send model part and footer
  http.getStream().print(modelPart);
  http.getStream().print(footer);

  // Finish the POST request
  int httpCode = http.GET();  // Strange usage but necessary with this
                              // HTTPClient library version
  String response;
  if (httpCode == HTTP_CODE_OK) {
    response = http.getString();
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, response);
    audioFile.close();
    http.end();  // Close connection

    if (doc.containsKey("text")) {
      return doc["text"].as<String>();
    } else {
      return "No text found in response";
    }
  } else {
    audioFile.close();
    http.end();  // Close connection
    Serial.println("Failed to complete the request");
    return response = http.getString();
  }
}

#endif OPENAI_H