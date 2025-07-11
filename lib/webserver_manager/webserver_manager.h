#pragma once
#include <ESPAsyncWebServer.h>
#include <config.h>
#include <ArduinoJson.h>
#include <AsyncJson.h>
#include <HTTPClient.h>
// #include <sensor_manager/sensor_manager.h>
#include <AsyncTCP.h>
#include <ElegantOTA.h>

#define SUPABASE_URL "https://ewlulhkrefobaooxmctt.supabase.co"
#define SUPABASE_API_KEY "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6ImV3bHVsaGtyZWZvYmFvb3htY3R0Iiwicm9sZSI6ImFub24iLCJpYXQiOjE3MzM4NDM5ODIsImV4cCI6MjA0OTQxOTk4Mn0.LGGDDHaAcH4f645jT3IC5-adPSku4BbRip52-Ui6e08"

class WebServerManager
{
private:
    AsyncWebServer server;
    Config &config;
    // SensorManager &sensorManager;

    bool isConnectedToWiFi = false;
    const unsigned long WIFI_CONNECT_TIMEOUT = 10000; // 10 seconds
    bool checkUserCredentials(const String &email, const String &password);

public:
    // WebServerManager(Config &cfg, SensorManager &sensorMgr, int &lcd_menu);
    WebServerManager(Config &cfg);
    void begin();
};