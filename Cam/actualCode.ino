#include "esp_camera.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

// ===================
// Select camera model
// ===================
#define CAMERA_MODEL_AI_THINKER // Has PSRAM
#include "camera_pins.h"

// WiFi credentials
const char* ssid = "Atuda";
const char* password = "1234512345";

// Firebase URL for updating the STREAM URL (new!)
#define CAMERA_UPDATE_URL "https://yellow-vortex-default-rtdb.europe-west1.firebasedatabase.app/commands/stream/stream_url.json"

// Vercel proxy URL to send (instead of local IP)
const char* vercelStreamURL = "https://functions-topp8abfj-doridondon25s-projects.vercel.app/stream";

// Function declarations
void startCameraServer();
void updateStreamURL();

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  // Camera configuration
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG; // For streaming
  config.frame_size = FRAMESIZE_UXGA;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count = 1;

  if (psramFound()) {
    config.jpeg_quality = 10;
    config.fb_count = 2;
    config.grab_mode = CAMERA_GRAB_LATEST;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.fb_location = CAMERA_FB_IN_DRAM;
  }

  // Initialize the camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x\n", err);
    return;
  }

  // WiFi connection
  WiFi.begin(ssid, password);
  WiFi.setSleep(false);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");

  startCameraServer();

  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");

  delay(1000);

  // Initial update
  updateStreamURL();
}

void loop() {
  // Retry updating the stream URL every 30 seconds (can adjust)
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected. Reconnecting...");
    WiFi.reconnect();
    delay(5000); // Allow some time for reconnection
  }

  updateStreamURL();
  delay(30000); // Update every 30 seconds
}

void updateStreamURL() {
  Serial.println("Updating stream URL...");
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure client;
    client.setInsecure(); // Insecure for quick testing

    HTTPClient https;
    String url = CAMERA_UPDATE_URL;

    Serial.print("Connecting to: ");
    Serial.println(url);

    if (https.begin(client, url)) {
      https.addHeader("Content-Type", "application/json; charset=UTF-8");

      // Push the Vercel Stream URL
      String jsonData = String("\"") + vercelStreamURL + String("\""); // just a string

      Serial.print("Payload: ");
      Serial.println(jsonData);

      int httpResponseCode = https.PUT(jsonData);

      if (httpResponseCode > 0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        Serial.println("Response payload:");
        Serial.println(https.getString());
      } else {
        Serial.print("HTTP error: ");
        Serial.println(https.errorToString(httpResponseCode));
      }

      https.end();
    } else {
      Serial.println("Failed to initialize HTTPS connection.");
    }
  } else {
    Serial.println("WiFi not connected. Skipping update.");
  }
}
