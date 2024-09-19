/**
 * Header       ESP32AutoConnect.h
 * 
 * Author       2024-09-18 Charles Geiser (https://www.dodeka.ch)
 * 
 * Purpose      Declaration of the class ESP32AutoConnect           
 */

#include "Arduino.h"
#include <ESPAsyncWebServer.h>
#include <Preferences.h>

#pragma once

class ESP32AutoConnect 
{
    public:
        ESP32AutoConnect(AsyncWebServer& server, Preferences& prefs, String accessPointSSID="AutoConnectAP") : 
            _server(server), _prefs(prefs), _apSSID(accessPointSSID) 
        {
        }

        void autoConnect();
        void clearCredentials();
        void setESPhostname(String hostname);
        
    private:
        bool credentialsAreAvailable();
        bool weAreConnectedToWLAN(String ssid, String password);
        void requestCredentialsAndRestart();
        String composeNetworkList();
        String _apSSID;
        String _apPassword;
        String _ssid;
        String _password;
        String _hostname = "esp-websrv";
        Preferences& _prefs;
        AsyncWebServer& _server;      
};