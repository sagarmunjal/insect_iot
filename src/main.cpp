#include <WiFi.h>
#include <ArduinoHttpClient.h>
#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <ArduinoCloud.h>

// Replace these with your credentials
const char* ssid = "Airtel_Virus2.o";
const char* password = "$h@lly@2024";

// Declare the built-in LED
const int builtinLED = 2;

// Define endpoints
const char* ledOnEndpoint = "/led-on";
const char* ledOffEndpoint = "/led-off";
const char* scanEndpoint = "/scan";

WiFiClient wifiClient;
HttpClient httpClient(wifiClient, "api.your-server.com", 80);

void setup() {
  Serial.begin(115200);
  pinMode(builtinLED, OUTPUT);
  
  // Initialize Wi-Fi as Access Point
  WiFi.softAP("ESP32_Access_Point");
  Serial.println("Access Point started");

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to Wi-Fi");

  // Initialize OTA
  ArduinoOTA.begin();
  
  // Initialize Arduino Cloud
  ArduinoCloud.begin("b89fbd0e-691f-46be-8875-372f3318900d", "KB1eV!QTe7qhvTYGRbWDp9a9v");
}

void loop() {
  ArduinoOTA.handle();
  
  // Check for incoming HTTP requests
  HttpRequest request = httpClient.getRequest();
  String requestPath = request.path();
  
  if (requestPath == ledOnEndpoint) {
    digitalWrite(builtinLED, HIGH);
    request.sendResponse(200, "LED turned ON");
  } else if (requestPath == ledOffEndpoint) {
    digitalWrite(builtinLED, LOW);
    request.sendResponse(200, "LED turned OFF");
  } else if (requestPath == scanEndpoint) {
    int numNetworks = WiFi.scanNetworks();
    String response = "Available Networks:\n";
    for (int i = 0; i < numNetworks; i++) {
      response += WiFi.SSID(i) + " (RSSI: " + String(WiFi.RSSI(i)) + ")\n";
    }
    request.sendResponse(200, response);
  } else {
    request.sendResponse(404, "Not Found");
  }

  delay(1000);
}