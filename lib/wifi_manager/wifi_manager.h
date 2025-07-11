#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <lcd_manager.h>

#define AQUAWATCH_WIFI_CONNECT_TIMEOUT 15000 // 15 seconds timeout for WiFi connection

class WiFiManager
{
private:
    LCDManager &lcd;

public:
    WiFiManager(LCDManager &lcd);
    void begin();
    void connect(const char *ssid, const char *password);
    bool isConnected();
};
