//Libraries included
#include <WiFi.h>
#include <WebServer.h>

// Define maximum length for SSID and password
#define MAX_SSID_LEN 32
#define MAX_PASS_LEN 64
char ssid[MAX_SSID_LEN];
char password[MAX_PASS_LEN];

//Declarations
const int blueLedPin = 2;  // Pin for the blue LED
const int redLedPin = 4;   // Pin for the red LED
WebServer server(80);

  //const char* ssid = "Airtel_Virus.2o";     // WiFi SSID
  //const char* password = "$h@lly@2024";     // WiFi Password

  const char* apSSID = "ESP32-AP";         // Access Point SSID
  const char* apPassword = "123456789";    // Access Point Password

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  pinMode(blueLedPin, OUTPUT); // Set the blue LED pin as an output
  pinMode(redLedPin, OUTPUT);  // Set the red LED pin as an output

  // Connect to Wi-Fi
  // Prompt user for SSID and password
  Serial.println("Enter your Wi-Fi SSID: ");
  readSerialInput(ssid, MAX_SSID_LEN);
  Serial.println("Enter your Wi-Fi password: ");
  readSerialInput(password, MAX_PASS_LEN);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi:" + String(ssid));

    // blink led while trying to connect  
    while (WiFi.status() != WL_CONNECTED) {
      digitalWrite(redLedPin, HIGH); // Turn the red LED on
      delay(200);                     // On for a short time
      digitalWrite(redLedPin, LOW);  // Turn the red LED off
      delay(200);                     // Off for a short time
      Serial.print("O__O");
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

void readSerialInput(char *input, int maxLen) {
    int index = 0; // Initialize index to track the position in the input array
    
    while (true) {
        if (Serial.available() > 0) { // Check if data is available to read from the Serial Monitor
            char incomingChar = Serial.read(); // Read one character from the serial input
            
            // Check if the character is the newline character '\n', indicating the end of input
            if (incomingChar == '\n') {
                input[index] = '\0'; // Null-terminate the string to mark the end of input
                break; // Exit the loop since the input is complete
            } else if (index < maxLen - 1) { // Ensure we don't exceed the buffer size
                input[index] = incomingChar; // Store the incoming character in the input array
                index++; // Move to the next position in the array
            }
        }
    }
}
