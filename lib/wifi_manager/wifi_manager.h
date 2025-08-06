#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <lcd_manager.h>

#define AQUAWATCH_WIFI_CONNECT_TIMEOUT 15000 // 15 seconds timeout for WiFi connection

class WiFiManager
{
private:
    LCDManager &lcd;

    // static IP
    IPAddress local_ip = IPAddress(192, 168, 1, 102);
    IPAddress gateway = IPAddress(192, 168, 1, 1);
    IPAddress subnet = IPAddress(255, 255, 255, 0);
    IPAddress primary_dns = IPAddress(192, 168, 1, 1);
    IPAddress secondary_dns = IPAddress(0, 0, 0, 0);

public:
    WiFiManager(LCDManager &lcd);
    void begin();
    void connect(const char *ssid, const char *password);
    void connect(const char *ssid, const char *password, bool *isConnecting);
    bool isConnected();
};
