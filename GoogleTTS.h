#ifndef GOOGLE_TTS_H
#define GOOGLE_TTS_H

#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>

#include "Audio.h"
#include "Utils.h"

const String googleAPIKey = "";
const String googleURL = "https://texttospeech.googleapis.com/v1/";

void googleTextToSpeech(const String& text) {
  String payload =
      String("{\"input\":{\"text\":\"") + text +
      "\"},\"voice\":{\"languageCode\":\"en-US\",\"ssmlGender\":\"FEMALE\"}," +
      "\"audioConfig\":{\"audioEncoding\":\"MP3\", "
      "\"sampleRateHertz\":16000}}";

  String response = makeHTTPSRequest(
      "POST", googleURL + "text:synthesize?key=" + googleAPIKey, payload,
      "application/json");

  // Parse the response
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, response);
  String audioContent =
      doc["audioContent"];  // Assuming 'audioContent' holds relevant data
  Serial.println("Audio content received:");
  Serial.println(audioContent);

  if (saveDecodedAudio(audioContent, "/tts.mp3")) {
    Serial.println("Audio content saved successfully.");
    // playWAVFile("/tts.wav");
  } else {
    Serial.println("Failed to save audio content.");
  }
}

#endif GOOGLE_TTS_H