#ifndef BASE64_H
#define BASE64_H
#include <Arduino.h>

static const char b64_alphabet[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int b64_char_value(char base64char) {
  if (base64char >= 'A' && base64char <= 'Z') return base64char - 'A';
  if (base64char >= 'a' && base64char <= 'z') return base64char - 'a' + 26;
  if (base64char >= '0' && base64char <= '9') return base64char - '0' + 52;
  if (base64char == '+') return 62;
  if (base64char == '/') return 63;
  return -1;
}

int base64_decode(const char* input, unsigned char* output, int outputLen) {
  int decodedLen = 0, val = 0, valb = -8;
  for (size_t i = 0; input[i]; i++) {
    unsigned char byte = input[i];
    if (byte == '=') break;
    byte = b64_char_value(byte);
    if (byte == -1) break;
    val = (val << 6) + byte;
    valb += 6;
    if (valb >= 0) {
      output[decodedLen++] = (val >> valb) & 0xFF;
      valb -= 8;
    }
  }
  return decodedLen;
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