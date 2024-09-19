/**
 * Program      ESP remote control.cpp
 * Author       2024-09-18 Charles Geiser (https://www.dodeka.ch)
 * 
 * Purpose      The program shows the use of my simple ESP32AutoConnect library.
 * 
 *              If the program cannot establish a connection to the WLAN, an access point
 *              "AutoConnectAP" is started. The user connects his cell phone or a laptop
 *              to this AP and opens the url 192.168.4.1. He is asked for the credentials
 *              of his WLAN until a connection is established and the web server is started. 
 *              Now the user opens the url http://esp-websrv, which then displays the website
 *              for remote control of his application. In the example, this application only 
 *              displays the time and date.  
 * 
 * Board        ESP32 DoIt DevKit V1
 *
 * Wiring       -
 *
 * Remarks      To keep the things simple, the credentials for the WLAN are stored 
 *              in the "Preferences" and it is assumed that the router assigns a 
 *              IP address to the esp web server by means of the dhcp.
 *
 * References   https://m1cr0lab-esp32.github.io/remote-control-with-websocket
 *              https://github.com/alanswx/ESPAsyncWiFiManager
 *              https://github.com/me-no-dev/ESPAsyncWebServer
 *              https://github.com/aliffathoni/ESPAutoWifi
 *              https://randomnerdtutorials.com/esp32-wi-fi-manager-asyncwebserver 
 */

#include <Arduino.h>
#include <Preferences.h>
#include "ESP32AutoConnect.h"
#include "index.h"

extern void heartbeat(uint8_t pin, unsigned long nBeats, unsigned long nSecs, unsigned long dutyOrPulseWidth, bool duty=true);
extern void printNearbyNetworks();
extern void printConnectionDetails();

const String  HOSTNAME = "esp-websrv";
const String  SSID_AP  = "esp-portal";

Preferences prefs; // holds the WiFi credentials
AsyncWebServer server(80);


/**
 * Handle the web user interface
 */
void handleWebUi()
{
    server.on("/", 
              HTTP_GET, 
              [](AsyncWebServerRequest *request) 
                {
                  request->send_P(200, "text/html", index_html);
                }
             );
}


void notFound(AsyncWebServerRequest *request) 
{
    request->send(404, "text/plain", "Not found");
}


void setup() 
{
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);

  ESP32AutoConnect ac(server, prefs); // create the autoconnect object, name of access point defaults to AutoConnectAP
  //ESP32AutoConnect ac(server, prefs, SSID_AP); // name of access point is set to SSID_AP = esp-portal

  ac.setESPhostname(HOSTNAME); // set a custom hostname for the web server

  //ac.clearCredentials(); // activate this line to start with deleted credentials
  ac.autoConnect(); // start the autoconnect process, continues on success

  // handle the main web page (index_html[] stored in index.h)
  handleWebUi(); 
 
  // start the web server, the user opens http://esp-websrv in a browser
  server.onNotFound(notFound);
  server.begin();

  printNearbyNetworks();
  printConnectionDetails();
}

void loop() 
{
  heartbeat(LED_BUILTIN, 1, 1, 5); // indicates that the loop is running
}
