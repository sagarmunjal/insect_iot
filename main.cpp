//Libraries included
#include <WiFi.h>

//Declarations
const int builtinLED = 2;

  const char* ssid = "Airtel_Virus.2o";     // WiFi SSID
  const char* password = "$h@lly@2024";     // WiFi Password

  const char* apSSID = "ESP32-AP";         // Access Point SSID
  const char* apPassword = "123456789";    // Access Point Password

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  pinMode(builtinLED, OUTPUT);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi:" + String(ssid));
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi:" + String(ssid));
  Serial.print("IP address: " + String(WiFi.localIP()));

  // Start the Access Point
  WiFi.softAP(apSSID, apPassword);
  Serial.print("Access Point:" + String(apSSID) + "started");
}

void loop() {
  // put your main code here, to run repeatedly:
  if(WiFi.status() == WL_CONNECTED){
    digitalWrite(builtinLED, HIGH);
    delay(500);
    digitalWrite(builtinLED, LOW);
    delay(500);
  }
}
