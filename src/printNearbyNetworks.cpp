#include <Arduino.h>
#include <WiFi.h>

/**
 * Print nearby WiFi networks with SSID und RSSI 
 */
void printNearbyNetworks()
{
  int n = WiFi.scanNetworks();
  Serial.printf(R"(
Nearby WiFi networks
--------------------
)");
  for (int i = 0; i < n; i++)
  {
    Serial.printf("%s\t%d\r\n", WiFi.SSID(i).c_str(), WiFi.RSSI(i));
  }
  Serial.println();
}