#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "Galaxy A51 241D";
const char* password = "pbut8223";

int movieID;
String movieName;
int duration;
String overview;

const int digitPins[] = {15, 2};
const int segmentPins[] = {0, 12, 13, 27, 14, 4, 21, 19};
//                         A   B   C   D   E  F   G  DP
const uint8_t charSegments[36][7] = {
  // 0-9
  {1, 1, 1, 1, 1, 1, 0}, // 0
  {0, 1, 1, 0, 0, 0, 0}, // 1
  {1, 1, 0, 1, 1, 0, 1}, // 2
  {1, 1, 1, 1, 0, 0, 1}, // 3
  {0, 1, 1, 0, 0, 1, 1}, // 4
  {1, 0, 1, 1, 0, 1, 1}, // 5
  {1, 0, 1, 1, 1, 1, 1}, // 6
  {1, 1, 1, 0, 0, 0, 0}, // 7
  {1, 1, 1, 1, 1, 1, 1}, // 8
  {1, 1, 1, 1, 0, 1, 1}, // 9
  // A-Z
  {1, 1, 1, 0, 1, 1, 1}, // A
  {0, 0, 1, 1, 1, 1, 1}, // b
  {1, 0, 0, 1, 1, 1, 0}, // C
  {0, 1, 1, 1, 1, 0, 1}, // d
  {1, 0, 0, 1, 1, 1, 1}, // E
  {1, 0, 0, 0, 1, 1, 1}, // F
  {1, 0, 1, 1, 1, 1, 0}, // G
  {0, 1, 1, 0, 1, 1, 1}, // H
  {0, 0, 1, 0, 0, 0, 1}, // I
  {0, 1, 1, 1, 1, 0, 0}, // J
  {1, 0, 1, 0, 1, 1, 1}, // K 
  {0, 0, 0, 1, 1, 1, 0}, // L
  {1, 1, 1, 0, 1, 0, 1}, // M 
  {1, 1, 1, 0, 1, 1, 0}, // N
  {1, 1, 1, 1, 1, 1, 0}, // O
  {1, 1, 0, 0, 1, 1, 1}, // P
  {1, 1, 1, 0, 0, 1, 1}, // Q
  {1, 1, 0, 0, 1, 1, 0}, // R
  {1, 0, 1, 1, 0, 1, 1}, // S
  {0, 0, 0, 1, 1, 1, 1}, // T
  {0, 1, 1, 1, 1, 1, 0}, // U
  {0, 1, 1, 1, 1, 1, 0}, // V 
  {0, 1, 1, 1, 0, 1, 1}, // W 
  {1, 1, 1, 0, 1, 1, 1}, // X 
  {0, 1, 1, 1, 0, 1, 1}, // Y
  {1, 1, 0, 1, 1, 0, 1}  // Z 
};

void setup() {
  Serial.begin(115200);
  connectToWiFi();
  setupDisplay();
}

void loop() {
  if (Serial.available() > 0) {
    movieID = Serial.parseInt();
    if(movieID > 0) {
      fetchMovieData(movieID);
    }
  }
}

void connectToWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void fetchMovieData(int id) {
  HTTPClient http;
  String url = "http://proiectia.bogdanflorea.ro/api/marvel-movies/movie?id=" + String(id);
  http.begin(url);

  int httpCode = http.GET();
  if (httpCode > 0) {
    String payload = http.getString();
    parseJSON(payload);
  } else {
    Serial.println("Error on HTTP request");
  }
  http.end();
}

void parseJSON(String json) {
  DynamicJsonDocument doc(2048);
  deserializeJson(doc, json);

  movieName = doc["title"].as<String>();
  duration = doc["duration"].as<int>();
  overview = doc["overview"].as<String>();

  displayData();
}

void setupDisplay() {

  for (int i = 0; i < 8; i++) {
    pinMode(segmentPins[i], OUTPUT);
    digitalWrite(segmentPins[i], LOW); 
  }

  for (int i = 0; i < 2; i++) {
    pinMode(digitPins[i], OUTPUT);
    digitalWrite(digitPins[i], LOW); 
  }
}

void displayData() {
  Serial.println("PIA - Echipa 223");

  scrollText(String(movieID), 1);
Serial.println();
  delay(2000);

  scrollText(movieName, 2);
Serial.println();
  delay(2000);

  scrollText(String(duration), 3);
Serial.println();
  delay(2000);

  Serial.print("ID: " + String(movieID));
  Serial.print(" | Title: " + movieName);
 // Serial.print(" | Duration: " + String(duration) + " min.");
  Serial.print('\n');
  Serial.print("  Descriere: " + overview);
  Serial.print('\n');

  Serial.println("Universul Marvel");
  Serial.print("WiFi: ");
  Serial.print(ssid);
  Serial.print(" | RSSI: ");
  Serial.print(WiFi.RSSI());
  Serial.println(" dB");
}

void scrollText(String text, int paramNumber) {
  String displayText = " " + text + " ";
  for (int i = 0; i < displayText.length() - 1; i++) {
    setCharacters(displayText[i], displayText[i + 1]);
   if(i!=0) printSerial(String(paramNumber));
    delay(330);
  }
}

void printSerial(String paramNumber) {
Serial.print(paramNumber); 
//  Serial.println();
}

void setCharacters(char c1, char c2) {
  uint8_t index1 = getCharIndex(c1);
  uint8_t index2 = getCharIndex(c2);

  if (index1 != 0xFF) {
    setDigit(0, index1);
  } else {
    clearDigit(0); 
  }

  if (index2 != 0xFF) {
    setDigit(1, index2);
  } else {
    clearDigit(1);
  }
}

void setDigit(int digit, uint8_t index) {
  digitalWrite(digitPins[digit], HIGH);
  for (int i = 0; i < 7; i++) {
    digitalWrite(segmentPins[i], charSegments[index][i]);
  }
  delay(1);
  digitalWrite(digitPins[digit], LOW);
}

void clearDigit(int digit) {
  digitalWrite(digitPins[digit], HIGH);
  for (int i = 0; i < 7; i++) {
    digitalWrite(segmentPins[i], LOW);
  }
  delay(1);
  digitalWrite(digitPins[digit], LOW);
}

uint8_t getCharIndex(char c) {
  if (c >= '0' && c <= '9') {
    return c - '0';
  } else if (c >= 'A' && c <= 'Z') {
    return c - 'A' + 10;
  } else if (c >= 'a' && c <= 'z') {
    return c - 'a' + 10;
  }
  return 0xFF;
}
