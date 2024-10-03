//Libraries included
#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>  // Library for storing Wi-Fi credentials

// Define maximum length for SSID and password
#define MAX_SSID_LEN 32
#define MAX_PASS_LEN 64
char ssid[MAX_SSID_LEN];
char password[MAX_PASS_LEN];
char command[MAX_SSID_LEN];

Preferences preferences; //Preferences object to save WiFi credentials

//Declarations
const int blueLedPin = 2;  // Pin for the blue LED
const int redLedPin = 4;   // Pin for the red LED
const char* apSSID = "ESP32-AP";         // Access Point SSID
const char* apPassword = "123456789";    // Access Point Password
unsigned long previousMillis = 0; // Store the last time WiFi status was checked
const long interval = 30000; // Interval to check WiFi status (10 seconds)
WebServer server(80);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(1000);
  pinMode(blueLedPin, OUTPUT); // Set the blue LED pin as an output
  pinMode(redLedPin, OUTPUT);  // Set the red LED pin as an output

  // setup WiFi
  connectToWiFi();   // Connect to Wi-Fi

  // Start the Access Point
  WiFi.softAP(apSSID, apPassword);
  Serial.println("Access Point:" + String(apSSID) + "started");
}

void loop() {
  // put your main code here, to run repeatedly:
  // check for serial input command
  if (Serial.available() > 0) {
    // Read the command into a buffer
    char command[MAX_SSID_LEN];
    handleAndReadSerial(command, MAX_SSID_LEN);  // This function reads the input
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

  // Check Wi-Fi connection status periodically
  unsigned long currentMillis = millis(); // Get the current time in milliseconds
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis; // Update previousMillis to the current time

    if (printWiFiStatus()) {
      Serial.println("WiFi connection lost. Attempting to reconnect...");
      connectToWiFi(); // Attempt to reconnect to Wi-Fi
    }
  }
}

// Function to handle serial input commands and read serial input

void handleAndReadSerial(char* inputBuffer, size_t maxLength) {
  char command[MAX_SSID_LEN];  // Buffer for command input
  size_t index = 0;

  // Wait until there's data available on Serial
  while (Serial.available() > 0 && index < maxLength - 1) {
    char c = Serial.read();
    if (c == '\n') {
      break; // Stop reading at newline character
    }
    command[index++] = c;
  }

  command[index] = '\0'; // Null-terminate the string
  // Check the command and trigger the appropriate function
  if (strcmp(command, "scan") == 0) {
    scanNetworks();  // Call the function to scan networks
  } else if (strcmp(command, "wifi-status") == 0) {
    printWiFiStatus();  // Call the function to check Wi-Fi status
  } else {
    Serial.println("Unknown command. Please use 'scan' or 'wifi-status'.");
  }
}

// Function to scan available network

void scanNetworks(){
  Serial.println("Scannning for available networks...");
  int networkCount = WiFi.scanNetworks();
  if(networkCount > 0){
    Serial.printf("%d network(s) found:\n", networkCount);
    for(int i = 0; i < networkCount; ++i){
      Serial.printf("%d: SSID: %s, RSSI: %d dBm, Encryption: %s\n", 
      i+1,
      WiFi.SSID(i).c_str(),
      WiFi.RSSI(i),
      getEncryptionType(WiFi.encryptionType(i))
      );
    }
  }
  // Clean up to free memory
  WiFi.scanDelete();
}

// Function to return encryption type as a string
const char* getEncryptionType(wifi_auth_mode_t encryptionType) {
  switch (encryptionType) {
    case WIFI_AUTH_OPEN: return "Open";
    case WIFI_AUTH_WEP: return "WEP";
    case WIFI_AUTH_WPA_PSK: return "WPA";
    case WIFI_AUTH_WPA2_PSK: return "WPA2";
    case WIFI_AUTH_WPA_WPA2_PSK: return "WPA/WPA2";
    case WIFI_AUTH_WPA2_ENTERPRISE: return "WPA2 Enterprise";
    default: return "Unknown";
  }
}

// Function to connect to Wi-Fi
bool connectToWiFi() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("No saved Wi-Fi credentials or connection failed. Prompting user...");
    getWiFiDetails(); // Call the new function to enter credentials
  }else{
    Serial.println("Checking Wi-Fi connection...");
    preferences.begin("wifiCreds", false);  // Start preferences
    String savedSSID = preferences.getString("ssid",""); // retrieve saved ssid
    String savedPassword = preferences.getString("password", ""); // retrieve saved pwd
    if (savedSSID.length() > 0 && savedPassword.length() > 0){
      //try if WiFi credentials already saved
      Serial.println("Connecting to saved WiFi network... " + String(savedSSID));
      WiFi.begin(savedSSID.c_str(), savedPassword.c_str());
    }
    printWiFiStatus();
  }

  preferences.end();
  return false;
}


// Function to print Wi-Fi status
bool printWiFiStatus() {
  // Check Wi-Fi status
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    //Blink red light while establishing connection
    digitalWrite(redLedPin, HIGH); // Turn the red LED on
    delay(100);                     // On for a short time
    digitalWrite(redLedPin, LOW);  // Turn the red LED off
    delay(200);                     // Off for a short time
    Serial.print("...searching for WiFi - attempt : " + String(attempts));
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected to Wi-Fi!" + String(WiFi.SSID()));
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    // Display signal strength as stars
    long rssi = WiFi.RSSI();
    int strength = map(rssi, -90, -30, 0, 8); // Map RSSI to a scale of 0 to 8
    Serial.print("Strength : ");
    for (int i = 0; i < strength; i++) {
      Serial.print("*");
    }
    for (int i = strength; i < 8; i++) {
      Serial.print("-");
    }
    return true;
  } else {
    Serial.println("No saved Wi-Fi found.");
    return false;
  }
}


void getWiFiDetails(){
  char ssid[MAX_SSID_LEN];
  char password[MAX_PASS_LEN];

  //Prompt user to enter WiFi credentials
  Serial.println("Enter your WiFi SSID: ");
  handleAndReadSerial(ssid, MAX_SSID_LEN);
  Serial.println("Enter your WiFi password: ");
  handleAndReadSerial(password, MAX_PASS_LEN);

  // Save WiFi credentials using preferences library
  preferences.begin("wifiCreds", false); // Begin preferences in read write mode
  preferences.putString("ssid", ssid);
  preferences.putString("password", password);
  preferences.end(); 

  Serial.println("WiFi credentials saved successfully");
   // Attempt to connect to Wi-Fi with the new credentials
  if (connectToWiFi()) {
    Serial.println("Successfully connected to Wi-Fi with new credentials.");
  } else {
    Serial.println("Failed to connect to Wi-Fi with the entered credentials.");
  }
}
