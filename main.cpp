//Libraries included
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoOTA.h>
#include <Preferences.h>  // Library for storing Wi-Fi credentials
Preferences preferences; //Preferences object to save WiFi credentials

// Define maximum length for SSID and password
#define MAX_SSID_LEN 32
#define MAX_PASS_LEN 64
char ssid[MAX_SSID_LEN];
char password[MAX_PASS_LEN]; 
bool isWiFiConnecting = false; //default false global scope
const char* apSSID = "ESP32-AP";         // Access Point SSID
const char* apPassword = "123456789";    // Access Point Password
bool stateAP = false;
String debugLog = "";
//give pins name
const int relayPin = 23;
const int moisturePin1 = 35;  // New moisture sensor 1
const int moisturePin2 = 32;  // New moisture sensor 2
const int moisturePin3 = 33;  // New moisture sensor 3
const int moisturePin4 = 34;  // New moisture sensor 4

WebServer server(80);  // Initialize the server on port 80

void setup() {
  Serial.begin(115200);
  delay(5000);

  WiFi.softAP(apSSID, apPassword);  // Create an access point
  stateAP = true;
  logDebugF("Booting....");
  // setup WiFi
  if(!connectToWiFi()){
    logDebugF("Connection failed, requesting new credentials...");
    getWiFiDetails();
  }
  //set pinmode
  pinMode(relayPin, OUTPUT);  // Set pin as output
  pinMode(moisturePin1, INPUT);  // Set pin as input
  pinMode(moisturePin2, INPUT);
  pinMode(moisturePin3, INPUT);
  pinMode(moisturePin4, INPUT);
  digitalWrite(relayPin, HIGH);  // Set HIGH to keep relay off
  logDebugF("Relay initialized to OFF state (HIGH).");
  serverRoutes();
  server.begin();
  ArduinoOTASetup(); // setup OTA
}

void loop() {
  // put your main code here, to run repeatedly:
  if(!WiFi.isConnected() && !isWiFiConnecting){
    logDebugF("WiFi is not connected, reconnecting..");
    connectToWiFi();
  }

  delay(1000);
  controlRelay();
  server.handleClient();
  ArduinoOTA.handle();
}
//Main Endpoints
void serverRoutes(){
  //homepage
  server.on("/",[](){
    String page = 
      "<h1>ESP32 HOME PAGE</h1>"
      "<p>Check network status<a href='/network'>here</a></p>"
      "<p>Check moisture level status<a href='/moisture-levels'>here</a></p>"
      "<p>Check error codes and logs<a href='/debug'>here</a></p>";
    server.send(200,"text/html", page);
  });
  //moisture-levels
  server.on("/moisture-levels", [](){
    String page =
      "<h3>Moisture level info</h3>"
      "<div>Moisture data for Sensor 1 : <span id='moisture1'>loading...</span></div>"
      "<div>Percentage data for Sensor 1: <span id='percentage1'>loading...</span></div>"
      "<div>Moisture data for Sensor 2 : <span id='moisture2'>loading...</span></div>"
      "<div>Percentage data for Sensor 2: <span id='percentage2'>loading...</span></div>"
      "<div>Moisture data for Sensor 3 : <span id='moisture3'>loading...</span></div>"
      "<div>Percentage data for Sensor 3: <span id='percentage3'>loading...</span></div>"
      "<div>Moisture data for Sensor 4 : <span id='moisture4'>loading...</span></div>"
      "<div>Percentage data for Sensor 4: <span id='percentage4'>loading...</span></div>"
      "<canvas id='moistureChart' width='200' height='100'></canvas>"
      "<script src='https://cdnjs.cloudflare.com/ajax/libs/Chart.js/3.9.1/chart.min.js'></script>"
      "<script src='https://cdn.jsdelivr.net/npm/chartjs-adapter-date-fns@3.0.0'></script>"
      "<script>"
      "const ctx = document.getElementById('moistureChart').getContext('2d');"
      "const moistureChart = new Chart(ctx, {"
      " type: 'line',"
      " data: {labels: [], datasets: [{label: 'Moisture(%)', data: [], borderColor: 'blue', borderWidth: 2 }]},"
      " options: {scales: {x: {type: 'time', time: {unit: 'second', displayFormats: {second:'h:mm:ss a'}}},"
      "                    y:{beginAtZero:true, max:100}}}"
      "});"
      "function fetchMoistureData(){"
      " fetch('/moisture-data')"
      "   .then(response => response.json())"
      "   .then(data => {"
      "      let averageMoisture = (data.moisture1 + data.moisture2 + data.moisture3 + data.moisture4) / 4;"
      "      let averagePercentage = (data.percentage1 + data.percentage2 + data.percentage3 + data.percentage4) / 4;"
      "       document.getElementById('moisture1').innerText = data.moisture1;\n"
      "       document.getElementById('moisture2').innerText = data.moisture2;\n"
      "       document.getElementById('moisture3').innerText = data.moisture3;\n"
      "       document.getElementById('moisture4').innerText = data.moisture4;\n"
      "       document.getElementById('percentage1').innerText = data.percentage1 + '%';\n"
      "       document.getElementById('percentage2').innerText = data.percentage2 + '%';\n"
      "       document.getElementById('percentage3').innerText = data.percentage3 + '%';\n"
      "       document.getElementById('percentage4').innerText = data.percentage4 + '%';"
      "       var now = new Date();"
      "       moistureChart.data.labels.push(now);"
      "       moistureChart.data.datasets[0].data.push(averagePercentage);"
      "       if(moistureChart.data.labels.length > 15) {"
      "         moistureChart.data.labels.shift();"
      "         moistureChart.data.datasets[0].data.shift();"
      "       }"
      "       moistureChart.update();"
      "    })"
      "   .catch(error => console.error('Error fetching data:', error));"
      "}"
      "setInterval(fetchMoistureData,2000);"
      "</script>";
    server.send(200, "text/html", page);
});
  //moisture-data
  server.on("/moisture-data",[](){        
    int moistureReading1 = analogRead(moisturePin1);
    int moistureReading2 = analogRead(moisturePin2);
    int moistureReading3 = analogRead(moisturePin3);
    int moistureReading4 = analogRead(moisturePin4);
    int moisturePercentage1 = convertToPercentage(moistureReading1);
    int moisturePercentage2 = convertToPercentage(moistureReading2);
    int moisturePercentage3 = convertToPercentage(moistureReading3);
    int moisturePercentage4 = convertToPercentage(moistureReading4);
    String jsonData = "{";
      jsonData += "\"moisture1\":" + String(moistureReading1) + ",";
      jsonData += "\"percentage1\":" + String(moisturePercentage1) + ",";
      jsonData += "\"moisture2\":" + String(moistureReading2) + ",";
      jsonData += "\"percentage2\":" + String(moisturePercentage2)+ ",";
      jsonData += "\"moisture3\":" + String(moistureReading3) + ",";
      jsonData += "\"percentage3\":" + String(moisturePercentage3)+ ",";
      jsonData += "\"moisture4\":" + String(moistureReading4) + ",";
      jsonData += "\"percentage4\":" + String(moisturePercentage4);
      jsonData += "}";
    server.send(200, "application/json", jsonData);
  });
  //debug-data
  server.on("/debug-data", []() {
    // Escape double quotes and newlines to prevent JSON syntax issues
    String escapedLog = debugLog;
    escapedLog.replace("\"", "\\\"");  // Escape double quotes
    escapedLog.replace("\n", "\\n");   // Escape newlines
    String jsonData = "{\"log\":\"" + escapedLog + "\"}";
    server.send(200, "application/json", jsonData);
  });
  //debug
  server.on("/debug",[](){
    String page = 
      "<pre id='log'> loading...  </pre>"
      "<form action='/clear' method='POST'>"
        "<button type='submit'>Clear Log</button>"
      "</form>"
      "<script>"
        "function updateLog(){"
        " fetch('/debug-data')"
        "   .then(response => {"
        "     if(!response.ok){"
        "       throw new Error('Internet connection unstable');"
        "     }"
        "     return response.json();"
        "   })"
        "   .then(data => {"
        "     console.log('Fetched data:', data);"
        "     document.getElementById('log').innerText = data.log;"
        "   })"
        "   .catch(error => console.error('Error fetching log:', error));"
        "}"
        "setInterval(updateLog, 2000);" // fetch updates every 2 minutes
        "updateLog();"
      "</script>";
    server.send(200, "text/html", page);
  });
  //clear
  server.on("/clear",[](){
    debugLog = "";
    server.send(200,"text/html","Log cleared! <a href='/debug'>Go back..!!</a>");
  });
  //config-wifi
  server.on("/config-wifi",[](){
    String page = 
      "<form action='/connect-wifi'>"
      "<input name='ssid' placeholder='SSID'><br>"
      "<input name='pass' placeholder='Password'><br>"
      "<input type='submit' value='Connect'></form>";
    server.send(200,"text/html", page);
  });
  //connect-wifi
  server.on("/connect-wifi",[](){
    String ssid = server.arg("ssid");
    String pass = server.arg("pass");

    WiFi.begin(ssid.c_str(),pass.c_str());
    server.send(200, "text/html", "connecting to WiFi....");
    logDebugF("Attempting to connect to: " + ssid);

    preferences.begin("wifiCreds", false);
    preferences.putString("ssid",ssid);
    preferences.putString("password",pass);
    preferences.end();
  });
  //network
  server.on("/network",[](){
    //Load available networks
    String page = "<h3>Available networks..</h3><ul>";
    int n = WiFi.scanNetworks();
    for(int i=0;i<n;i++){
      String network = WiFi.SSID(i);
      page += "<li><a href='/select?ssid=" + network + "'>" + network + "(" + String(WiFi.RSSI(i)) + "dBm)</a></li>";
    }
    page += "</ul>";
    //Show Network Connection status
    page += "<h3>Network Connection status</h3>";
    if(WiFi.isConnected()){
      page += "Connected to: " + WiFi.SSID() + "<br>Signal Strength: " + String(WiFi.RSSI()) + "dBm<br>";
    }else{
      page += "Not connected to any Network<br>";
    }
    page += "Access Point: " + String(stateAP ? "Active" : "Inactive");
    server.send(200, "text/html", page);
  });
  //select
  server.on("/select",[](){
    String ssid = server.arg("ssid");
    String page = 
      "<h3>Connect to: " + ssid + "</h3>"
      "<form action='/connect-wifi'>"
      "<input name='ssid' value='" + ssid + "' readonly><br>"
      "<input name='pass' placeholder='Password' type='password'><br>"
      "<input type='submit' value='connect'></form>";
    server.send(200,"text/html",page);
  });
}
//Main Functions
void controlRelay() {
  // Read moisture levels
  int moistureReading1 = analogRead(moisturePin1);
  int moistureReading2 = analogRead(moisturePin2);
  int moistureReading3 = analogRead(moisturePin3);
  int moistureReading4 = analogRead(moisturePin4);

  // Convert readings to percentage
  int moisturePercentage1 = convertToPercentage(moistureReading1);
  int moisturePercentage2 = convertToPercentage(moistureReading2);
  int moisturePercentage3 = convertToPercentage(moistureReading3);
  int moisturePercentage4 = convertToPercentage(moistureReading4);

  // Calculate average moisture level
  int averageMoisture = (moisturePercentage1 + moisturePercentage2 + moisturePercentage3 + moisturePercentage4) / 4;

  // Define thresholds
  int lowerThreshold = 30; // Lower limit
  int upperThreshold = 70; // Upper limit

  // Relay control logic
  if (averageMoisture < lowerThreshold) {
    digitalWrite(relayPin, LOW); // Turn ON the relay
    logDebugF("Soil is too dry. Relay turned ON.");
  } else if (averageMoisture > upperThreshold) {
    digitalWrite(relayPin, HIGH); // Turn OFF the relay
    logDebugF("Soil is too moist. Relay turned OFF.");
  }
}
void logDebugF(String message){
  Serial.println(message);
  debugLog += message + "\n";
}

bool connectToWiFi(){
  logDebugF("Checking Wi-Fi connection...");
  isWiFiConnecting = true; //default true local scope
  preferences.begin("wifiCreds", false);  // Start preferences
  String savedSSID = preferences.getString("ssid",""); // retrieve saved ssid
  String savedPassword = preferences.getString("password", ""); // retrieve saved pwd
  preferences.end();
  if (savedSSID.length() == 0 || savedPassword.length() == 0){
    logDebugF("No saved network found");
    isWiFiConnecting = false; //reset to false
    return false;
  }
  logDebugF("Saved network found: " + savedSSID + " ..Trying to reconnect");
  WiFi.begin(savedSSID.c_str(), savedPassword.c_str());
  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < 10000) {
    delay(500);  // Wait for connection (up to 10 seconds)
    logDebugF("Connecting...");
  }

  isWiFiConnecting = false;  // Reset flag
  if (WiFi.status() == WL_CONNECTED) {
    logDebugF("Local IP: " + WiFi.localIP().toString());
    logDebugF("Connected to Wi-Fi: " + savedSSID);
    return true;
  } else {
    logDebugF("Failed to connect.");
    return false;
  }
}

  void getWiFiDetails(){
      //Prompt user to enter WiFi credentials
      logDebugF("Enter your WiFi SSID: ");
  }
int convertToPercentage(int reading){
  int minReading = 1500;     // wet
  int maxReading = 4000;     // dry
  reading = constrain(reading, minReading, maxReading);
  return map(reading, minReading, maxReading, 100, 0); 
}
void ArduinoOTASetup(){
    ArduinoOTA
    .onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else {  // U_SPIFFS
      type = "filesystem";
    }

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    logDebugF("Start updating " + type);
  })
  .onEnd([]() {
    logDebugF("\nEnd");
  })
  .onProgress([](unsigned int progress, unsigned int total) {
    int percent = (progress*100) / total;
    logDebugF("Progress:  " + String(percent) + "%");
  })
  .onError([](ota_error_t error) {
    logDebugF("Error: "+ String(error));
    if (error == OTA_AUTH_ERROR) {
      logDebugF("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      logDebugF("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      logDebugF("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      logDebugF("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      logDebugF("End Failed");
    }
  });
  ArduinoOTA.begin();
}
