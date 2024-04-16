#ifndef BASE64_H
#define BASE64_H
#include <Arduino.h>

static const char b64_alphabet[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

// Function to decode a single Base64 character
int b64_char_value(char base64char) {
  if (base64char >= 'A' && base64char <= 'Z') return base64char - 'A';
  if (base64char >= 'a' && base64char <= 'z') return base64char - 'a' + 26;
  if (base64char >= '0' && base64char <= '9') return base64char - '0' + 52;
  if (base64char == '+') return 62;
  if (base64char == '/') return 63;
  return -1;
}

// Function to decode a Base64 string
int base64_decode(const char* input, unsigned char* output, int outputLength) {
  int i, j;
  int len = strlen(input);
  int pad = len > 0 && (len % 4 || input[len - 1] == '=');
  const char* ptr = input;

  outputLength = 0;

  for (i = 0; i < len; i += 4) {
    int n = b64_char_value(ptr[i]) << 18 | b64_char_value(ptr[i + 1]) << 12 |
            b64_char_value(ptr[i + 2]) << 6 | b64_char_value(ptr[i + 3]);
    for (j = 0; j < 3 && i * 3 / 4 + j < outputLength - pad; ++j) {
      output[i * 3 / 4 + j] = (n >> (16 - j * 8)) & 0xFF;
    }
  }
  return outputLength - pad;
}

// Encode a byte array into a Base64 string
String base64_encode(const unsigned char* input, int length) {
  String encodedString = "";
  int i;
  int modTable[] = {0, 2, 1};

  for (i = 0; i < length; i += 3) {
    int n = input[i] << 16;
    if (i + 1 < length) {
      n |= input[i + 1] << 8;
    }
    if (i + 2 < length) {
      n |= input[i + 2];
    }

    for (int j = 0; j < 4; j++) {
      if (i * 8 + j * 6 > length * 8) {
        encodedString += '=';
      } else {
        encodedString += b64_alphabet[(n >> (18 - j * 6)) & 63];
      }
    }
  }

  return encodedString;
}
#endif BASE64_H