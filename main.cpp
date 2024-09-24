//Libraries included
#include <WiFi.h>

//Declarations
const int blueLedPin = 2;  // Pin for the blue LED
const int redLedPin = 4;   // Pin for the red LED

  const char* ssid = "Airtel_Virus.2o";     // WiFi SSID
  const char* password = "$h@lly@2024";     // WiFi Password

  const char* apSSID = "ESP32-AP";         // Access Point SSID
  const char* apPassword = "123456789";    // Access Point Password

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  pinMode(blueLedPin, OUTPUT); // Set the blue LED pin as an output
  pinMode(redLedPin, OUTPUT);  // Set the red LED pin as an output

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi:" + String(ssid));

    // blink led while trying to connect  
    while (WiFi.status() != WL_CONNECTED) {
      digitalWrite(redLedPin, HIGH); // Turn the red LED on
      delay(250);                     // On for a short time
      digitalWrite(redLedPin, LOW);  // Turn the red LED off
      delay(250);                     // Off for a short time
      Serial.print(".");
    }
    // after WiFi is connected
    Serial.println("Connected to WiFi:" + String(ssid));
    digitalWrite(redLedPin, HIGH);
    Serial.print("IP address: " + String(WiFi.localIP()));

  // Start the Access Point
  WiFi.softAP(apSSID, apPassword);
  Serial.print("Access Point:" + String(apSSID) + "started");
}

void loop() {
  // put your main code here, to run repeatedly:
  //check WiFi connection staibility
  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(redLedPin, HIGH); // Keep red LED on if connected
  } else {
    digitalWrite(redLedPin, LOW);  // Turn off red LED if disconnected
  }

    // Check if the access point is active
  if (WiFi.softAPgetStationNum() > 0) {
    // Blink the blue LED if there are clients connected to the AP
    digitalWrite(blueLedPin, HIGH); // Turn the blue LED on
    delay(500);                     // On for a short time
    digitalWrite(blueLedPin, LOW);  // Turn the blue LED off
    delay(500);                     // Off for a short time
  } else {
    // No clients connected, keep the blue LED off
    digitalWrite(blueLedPin, LOW);
  }
}
