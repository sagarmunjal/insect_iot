//Libraries included
#include <WiFi.h>

//Declarations
const int builtinLED = 2;

  // Replace these with your credentials
  const char* ssid = "Airtel_Virus.2o";
  const char* password = "$h@lly@2024";


void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  pinMode(builtinLED, OUTPUT);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to Wi-Fi");

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
