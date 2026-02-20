/**
 * Class        Implementation of the class methods of ESP32AutoConnect
 * 
 * Author       2024-09-18 Charles Geiser (https://www.dodeka.ch)
 *              2026-01-09 Implementation of an effective WiFi restart in 
 *                         case of connection failure 
 * 
 * Purpose      Yet another ESP32 autoconnect WiFi library
 *              Starts up an access point AutoConnectAP when no WLAN connection 
 *              could be established and asks the user for WLAN credentials.
 * 
 * Usage        initESP32AutoConnect(AsyncWebServer *webServer, Preferences &prefs, const char hostname[])
 *              ac.clearCredentials;  // for test
 *              ac.autoconnect();
 * 
 *              - Connect your cell phone to AutoConnectAP
 *              - Open 192.168.4.1 in your browser and enter the WLAN credentials
 *              - Connect to your WLAN and open http://esp32-device to control
 *                your application remotely
 *              - The next time you start the program, your login details are 
 *                known and do not need to be entered again
 *              - Note that WiFi.setAutoReconnect(false) is set to “false” because 
 *                we will reconnect ourselves after an interruption using a timer
 * 
 * Board        ESP32 DoIt DevKit V1
 * Remarks
 * 
 * References   https://github.com/alanswx/ESPAsyncWiFiManager
 *              https://randomnerdtutorials.com/esp32-wi-fi-manager-asyncwebserver
 *              https://github.com/aliffathoni/ESPAutoWifi             
 */
#include "ESP32AutoConnect.h"
#include "query.h"
#include <Ticker.h>

const bool RW_MODE = false;
const bool RO_MODE = true;

Ticker wifiReconnectTimer;

/**
 * Looks for stored credentials and returns true
 * if both ssid and password are found
 */
bool ESP32AutoConnect::credentialsAreAvailable()
{
    _prefs.begin("CREDENTIALS", RO_MODE);
    _ssid = _prefs.getString("ssid", "");
    _password = _prefs.getString("password", "");
    _prefs.end();
    return (_ssid != "" && _password != "") ? true : false;    
}


/**
 * Clear the stored network
 */
void ESP32AutoConnect::clearCredentials()
{
  _prefs.begin("CREDENTIALS", RW_MODE);
  _prefs.clear();
  _prefs.end();
}


void onWiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info) {
  switch (event) {

    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
      Serial.printf("WiFi connected. IP: %s\n", WiFi.localIP().toString().c_str());
      break;

    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
      Serial.printf("WiFi disconnected, reason: %d\n", info.wifi_sta_disconnected.reason);

      // Try to reconnect to the Wi-Fi with a timer every 5 seconds
      // Be shure that automatic reconnect is disabled
      // WiFi.setAutoReconnect(false);
      wifiReconnectTimer.once(5, []() {
        Serial.println("Trying to reconnect WiFi...");
        WiFi.disconnect(false, false);
        WiFi.begin();
      });
      break;
  }
}


/**
 * Returns true if th ESP is connected to the network stored in preferences
 */
bool ESP32AutoConnect::weAreConnectedToWLAN(String ssid, String password) 
{  
  WiFi.setHostname(_hostname.c_str()); // set hostname first
  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);                // crucial for AsyncWebServer
  WiFi.setAutoReconnect(false);
  WiFi.persistent(false);
  WiFi.onEvent(onWiFiEvent);
  WiFi.begin(ssid, password);
  return (WiFi.waitForConnectResult() != WL_CONNECTED) ? false : true;
}


/**
 * Look for available networks and compose the 
 * option list for the query web page
 */
String ESP32AutoConnect::composeNetworkList()
{
  String networks = "";
  int n = WiFi.scanNetworks();
  for (int i = 0; i <n; i++)
  {
    // <option value="0">Dodeka2G4</option>
    char buf[64];
    snprintf(buf, sizeof(buf), "<option value=\"%d\">%s</option>", i, WiFi.SSID(i).c_str()); 
    networks += String(buf);
  }
  //log_i("%s", networks.c_str());
  return networks;
} 


/**
 * Starts an access point, searches for available networks and prompts the user
 * on the query web page to select a network and enter the corresponding password
 */
void ESP32AutoConnect::requestCredentialsAndRestart()
{
  Preferences& pr = _prefs;
  String& hn = _hostname;
  WiFi.disconnect();
  WiFi.softAP(_apSSID.c_str(), NULL);
  log_e("\n==> Connect your mobile to %s and \nenter your WLAN credentials on page http://%s", _apSSID.c_str(), WiFi.softAPIP().toString().c_str());

  String networks = composeNetworkList();
  String query = query0;  // raw literal from query.h
  query.replace("{n}", networks); // insert available networks into query web page

  _server->on("/", 
            HTTP_GET, 
            [query](AsyncWebServerRequest *request)
              {
                request->send(200, "text/html", query.c_str());
              });

  _server->on("/get",
            HTTP_GET,
            [&pr, hn] (AsyncWebServerRequest *request) 
            {
              String ssid, password;
              if (request->hasParam("ssid")) 
              {
                int index = (request->getParam("ssid")->value()).toInt();
                ssid = WiFi.SSID(index);
              }
              if (request->hasParam("password")) 
              {
                password = request->getParam("password")->value();
              }

              pr.begin("CREDENTIALS", RW_MODE);
              pr.putString("ssid", ssid);
              pr.putString("password", password);
              pr.end();

              /* request->send(200, "text/html", 
                                 "Credentials saved. ESP will restart and<br>connect to your WLAN "
                                 + ssid + ".<br>Proceed to http://" + hn); */
/*               request->send(200, "text/html",
                      "<div style='font-size:48px; font-family:Arial;'>"
                      "Credentials saved. ESP will restart and<br>"
                      "connect to your WLAN " + ssid + ".<br>"
                      "Proceed to http://" + hn +
                      "</div>"
                  ); */
/*               request->send(200, "text/html",
                  "<html>"
                  "<head>"
                  "<style>"
                  "body { font-size: 32px; font-family: Arial; padding: 20px; }"
                  "h1 { font-size: 48px; }"
                  "</style>"
                  "</head>"
                  "<body>"
                  "<h1>Credentials saved</h1>"
                  "ESP will restart and<br>"
                  "connect to your WLAN <b>" + ssid + "</b>.<br><br>"
                  "Proceed to <b>http://" + hn + "</b>"
                  "</body>"
                  "</html>"
              ); */
              request->send(200, "text/html",
                "<html>"
                "<head>"
                    "<style>"
                    "body { font-size: 24px; font-family: Arial; padding: 2rem; }"
                    ".box {"
                      "background:#fffae6;"
                      "border:1rem solid #e0c200;"
                      "padding:1rem;"
                      "border-radius:1rem;"
                      "max-width:500px;"
                    "}"
                    "</style>"
                "</head>"
                "<body>"
                    "<div class='box'>"
                    "<b>Credentials saved!</b><br><br>"
                    "ESP will restart and connect to your<br>"
                    "WLAN <b>" + ssid + "</b>.<br><br>"
                    "Proceed to <b>http://" + hn + "</b>"
                    "</div>"
                "</body>"
                "</html>"
            );

              // Neustart asynchron nach 2 Sekunden 
              esp_timer_handle_t restartTimer; 
              const esp_timer_create_args_t timerArgs = { 
                .callback = [](void*) { ESP.restart(); }, 
                .arg = nullptr, .dispatch_method = ESP_TIMER_TASK, 
                .name = "restart_timer"
              };
              esp_timer_create(&timerArgs, &restartTimer); 
              esp_timer_start_once(restartTimer, 5000000);
            });
  _server->begin();    
}


void notFound1(AsyncWebServerRequest *request) 
{
    request->send(404, "text/plain", "Not found");
}


/**
 * Start the autoconnect process
 */
void ESP32AutoConnect::autoConnect()
{
  bool notConnected = true;

  if (credentialsAreAvailable())
  {
    if (weAreConnectedToWLAN(_ssid, _password))
    {
      log_i("\n==> Connected to your WLAN %s. Proceed to http://%s or http://%s", _ssid, WiFi.getHostname(), WiFi.localIP().toString().c_str());
      notConnected = false;
    }
    else
    {
      log_e("==> Could not connect to your WLAN %s, try again or select another network.", _ssid.c_str());
      requestCredentialsAndRestart();
    }
  }
  else
  {
    log_e("==> No WiFi credentials available");
    requestCredentialsAndRestart();
  }
  // The next line prevents the main loop from being entered 
  // before a connection has been established. 
  while (notConnected) { delay(100); }  
}
