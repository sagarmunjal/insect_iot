v0.1 - Includes the setup, minimum software/hardware requirements and project execution. Vamos!

Conquering the ESP32: From iPad to Cloud with Arduino, OTA and a Touch of Magic Welcome back, fellow makers! Today, we're diving into the exciting world of ESP32 programming, pushing the boundaries of development by embracing a multi-faceted, cloud-connected approach.

This journey started with a simple goal: program an ESP32 DevKit while navigating the constraints of different development environments, from the mobile comfort of an iPad Air and Redmi 8 Pro to the power of a Google Cloud Platform (GCP) VM with Ubuntu and GitHub.dev codespaces facilitating seamless deployment. We've tackled several milestones, each adding a layer of sophistication to our ESP32 project.

Milestone 1: Arduino Cloud & OTA Bootloading

Our first objective was to unlock the power of Over-the-Air (OTA) bootloading for the ESP32, leveraging the user-friendly interface of Arduino Cloud. This meant we could update our ESP32 code wirelessly, without the need for physical connections!

Milestone 1 Sub-Tasks:

A - ESP32 as an Access Point: We transformed our ESP32 into a wireless access point, allowing us to connect to it directly and begin programming. This setup proved incredibly useful for initial configurations and debugging. 
B - WiFi Connection & Signal Strength: Our ESP32 learned to connect to existing Wi-Fi networks. Further, we used the RSSI (Received Signal Strength Indicator) to measure signal strength, providing valuable insight into network performance. We even created a /scan endpoint that returned a list of available networks, giving the ESP32 the ability to actively search for and join new Wi-Fi networks. 
C - The Magic LED: We took control of the onboard LED on GPIO pin 2, making it dance to our commands. By accessing /led-on and /led-off endpoints, we could turn the LED on and off, adding a visual confirmation to our ESP32's connectivity and functionality.

05092024 Logs 

yes now lets focus on creating the entire esp32 program from the beginning, step by step we have to acheive the following
stage A - Setting up the Environment
A1.  connect github.dev with an already created, new repository called insect_iot and push changes to its "testing" branch
A2 - install packages like pio and other packages in github.dev

Stage B - understand and write step by step program logic and code for the main.cpp file to do achieve the following 
B1 - OTA Arduino cloud updates
B2 - Inbuilt led blink (gpio2) when endpoint /led-on and /led-off are triggered
B3 - wifi related logics - 
B3.1 - use esp32 as access point 
B3.2 - implement RSSI , program /scan endpoint to scan available networks