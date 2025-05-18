#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"
#include <Adafruit_AHTX0.h>
#include <stdlib.h>

// ========== Wi-Fi Credentials ==========
#define WIFI_SSID "Atuda"
#define WIFI_PASSWORD "1234512345"

// ==========Endpoint URLs ==========
#define CAR_DIRECTION_URL "https://backend-0rd9.onrender.com/get-robot-direction"
#define ECG_UPDATE_URL "https://backend-0rd9.onrender.com/set-ecg"
#define Server_ip "https://backend-0rd9.onrender.com"
#define BPM_UPDATE_URL "https://backend-0rd9.onrender.com/set-bpm"
#define TEMP_UPDATE_URL "https://backend-0rd9.onrender.com/set-temperature"
#define HUMIDITY_UPDATE_URL "https://backend-0rd9.onrender.com/set-humidity"
#define UPDATE_ALL_URL "https://backend-0rd9.onrender.com/update-all-values"

// Debugging flag
#define DEBUG true

// ========== Car Control Pins ==========
const int motor1Pin1 = 2;
const int motor1Pin2 = 4;
const int motor2Pin1 = 5;
const int motor2Pin2 = 18;

// ========== Temperature & Humidity Sensor ==========
Adafruit_AHTX0 aht;

// ========== Heart Rate and ECG Sensor ==========
MAX30105 particleSensor;
const byte RATE_SIZE = 4; 
byte rates[RATE_SIZE];
byte rateSpot = 0;
long lastBeat = 0;
float beatsPerMinute;
int beatAvg;

// AD8232 ECG Sensor
const int ecgPin = 35;
const float maxVoltage = 3.3;
int ecgReadings[RATE_SIZE];
int ecgSpot = 0;
int ecgAvg = 0;

// ========== Timing Intervals ==========
unsigned long lastCommandCheck = 0;
unsigned long lastECGUpdate = 0;
unsigned long lastBPMUpdate = 0;
unsigned long lastEnvUpdate = 0;
const unsigned long commandInterval = 1000;
const unsigned long ecgBpmInterval = 10000;    
const unsigned long envInterval = 3000;

// ========== Wi-Fi Reconnection ==========
void reconnectWiFi() {
  if (DEBUG) Serial.println("Reconnecting to Wi-Fi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    if (DEBUG) Serial.print(".");
    delay(500);
  }
  if (DEBUG) Serial.println("\nReconnected to Wi-Fi!");
}

// ========== Car Control Functions ==========
void StopMotors() {
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, LOW);
  digitalWrite(motor2Pin1, LOW);
  digitalWrite(motor2Pin2, LOW);
}

void Forward() {
  digitalWrite(motor1Pin1, HIGH);
  digitalWrite(motor1Pin2, LOW);
  digitalWrite(motor2Pin1, LOW);
  digitalWrite(motor2Pin2, HIGH);
}

void Backward() {
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, HIGH);
  digitalWrite(motor2Pin1, HIGH);
  digitalWrite(motor2Pin2, LOW);
}

void Right() {
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, HIGH);
  digitalWrite(motor2Pin1, LOW);
  digitalWrite(motor2Pin2, HIGH);
}

void Left() {
  digitalWrite(motor1Pin1, HIGH);
  digitalWrite(motor1Pin2, LOW);
  digitalWrite(motor2Pin1, HIGH);
  digitalWrite(motor2Pin2, LOW);
}

void executeCommand(int command) {
  switch (command) {
    case 1:
      if (DEBUG) Serial.println("Moving Forward");
      Forward();
      break;
    case 2:
      if (DEBUG) Serial.println("Turning Right");
      Right();
      break;
    case 3:
      if (DEBUG) Serial.println("Moving Backward");
      Backward();
      break;
    case 4:
      if (DEBUG) Serial.println("Turning Left");
      Left();
      break;
    case 5:
      if (DEBUG) Serial.println("Stopping");
      StopMotors();
      break;
    default:
      if (DEBUG) Serial.println("Unknown Command");
      StopMotors();
      break;
  }
}

// ==========Update Functions ==========

void SendPost(int data, String endpoint)
{
  if (WiFi.status() != WL_CONNECTED) {
    reconnectWiFi();
  }
  HTTPClient http;

  http.begin(endpoint);
  http.addHeader("Content-Type", "application/json");
  String jsonData = String("{\"data\":") + data + String("}");
  int httpResponseCode = http.POST(jsonData);
  
  if (httpResponseCode == 200) {
    if (DEBUG) Serial.println(String("ECG updated successfully: ") + data);
  } else {
    if (DEBUG) Serial.println(String("Failed to update ECG. HTTP Response code: ") + httpResponseCode);
  }
  http.end();
}


void SendAll(int temperature,int humidity,int ecg,int bpm)
{
  if (WiFi.status() != WL_CONNECTED) {
    reconnectWiFi();
  }
  HTTPClient http;

  http.begin(UPDATE_ALL_URL);
  http.addHeader("Content-Type", "application/json");
  String jsonData = String("{");
  jsonData += String("\"temperature\":") + temperature + String(", ");
  jsonData += String("\"humidity\":") + humidity + String(", ");
  jsonData += String("\"ecg\":") + ecg + String(", ");
  jsonData += String("\"bpm\":") + bpm;
  jsonData += String("}");

  Serial.println(jsonData);
  int httpResponseCode = http.POST(jsonData);
  
  if (httpResponseCode == 200) {
    if (DEBUG) Serial.println(String("sent successfully"));
  } else {
    if (DEBUG) Serial.println(String("Failed to update All. HTTP Response code: ") + httpResponseCode);
  }
  http.end();
}


int GetDrive()
{
  if (WiFi.status() != WL_CONNECTED) {
    reconnectWiFi();
  }
  HTTPClient http;

  http.begin(CAR_DIRECTION_URL);
  
  int httpResponseCode = http.GET();
  
  String direction = "5";
  
  if (httpResponseCode == 200) {
    if (DEBUG) Serial.print(String("got direction successfully"));
    String body = http.getString();
    direction = body[14];
    Serial.println(direction);
  } else {
    if (DEBUG) Serial.println(String("Failed to get direction. HTTP Response code: ") + httpResponseCode);
  }
  http.end();


  int car_direction_int = 0;
      if(direction=="1")
      {
        car_direction_int = 1;
      }
      else if(direction=="2")
      {
        car_direction_int = 2;
      }
      else if(direction=="3")
      {
        car_direction_int = 3;
      }
      else if(direction=="4")
      {
        car_direction_int = 4;
      }
      else if(direction=="5")
      {
        car_direction_int = 5;
      }

  return car_direction_int;
}

// ========== Setup ==========
void setup() {
  Serial.begin(115200);
  if (DEBUG) Serial.println("Initializing system...");

  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(motor2Pin1, OUTPUT);
  pinMode(motor2Pin2, OUTPUT);
  StopMotors();

  Wire.begin(21, 22);

  if (!aht.begin()) {
    Serial.println("Could not find AHT10 sensor. Check wiring!");
    while (1) delay(10);
  }
  Serial.println("AHT10 sensor initialized.");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println("\nWi-Fi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // ECG and Heart Rate Sensor Setup
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    if (DEBUG) Serial.println("MAX30105 not found. Check wiring/power.");
    while (1);
  }
  particleSensor.setup();
  particleSensor.setPulseAmplitudeRed(0x0A);
  particleSensor.setPulseAmplitudeGreen(0);

  analogSetAttenuation(ADC_11db);
  analogReadResolution(12);
}

// ========== Main Loop ==========
void loop() {
  unsigned long currentMillis = millis();

  // Read and Update ECG and BPM every 10 sec
  if(currentMillis - lastECGUpdate > 10000)
  {
    lastECGUpdate = currentMillis; 
    int ecgValue = analogRead(ecgPin);
    if (ecgValue != 0) {
      ecgReadings[ecgSpot++] = ecgValue;
      ecgSpot %= RATE_SIZE;

      ecgAvg = 0;
      for (byte i = 0; i < RATE_SIZE; i++) {
        ecgAvg += ecgReadings[i];
      }
      ecgAvg /= RATE_SIZE;
    }
  }

  if(currentMillis - lastBPMUpdate > 5000)
  {
    lastBPMUpdate = currentMillis; 
    long irValue = particleSensor.getIR();
    if (checkForBeat(irValue)) {
      long delta = millis() - lastBeat;
      lastBeat = millis();
      beatsPerMinute = 60 / (delta / 1000.0);

      if (beatsPerMinute > 20 && beatsPerMinute < 255) {
        rates[rateSpot++] = (byte)beatsPerMinute;
        rateSpot %= RATE_SIZE;

        beatAvg = 0;
        for (byte i = 0; i < RATE_SIZE; i++) {
          beatAvg += rates[i];
        }
        beatAvg /= RATE_SIZE;
      }
    }
  }

  sensors_event_t humidity, temp;
  if (currentMillis - lastEnvUpdate >= envInterval) {
    lastEnvUpdate = currentMillis;
    aht.getEvent(&humidity, &temp);
  }

  Serial.println("ecg avg: " + ecgAvg);
  Serial.println(ecgAvg);
  Serial.println("beat avg: " + beatAvg);
  Serial.print("temp: ");
  Serial.println(temp.temperature);
  Serial.print("humidity: ");
  Serial.println(humidity.relative_humidity);
  


  // update all
  if (currentMillis - lastCommandCheck >= commandInterval) {
    lastCommandCheck = currentMillis;
    if (WiFi.status() == WL_CONNECTED) {
      SendAll(temp.temperature, humidity.relative_humidity, ecgAvg, beatAvg);
      int car_direction = GetDrive();
      

      executeCommand(car_direction);
      Serial.println("send stuff");
      }
     else {
      reconnectWiFi();
    }
  }

  delay(1000);  
}
