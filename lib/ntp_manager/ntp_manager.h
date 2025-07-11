#pragma once

#include <Arduino.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <WiFi.h>

class NTPManager
{
private:
    WiFiUDP ntpUDP;
    NTPClient *timeClient;
    unsigned long updateInterval;
    long timeOffset;
    unsigned long lastUpdateAttempt;
    bool ntpSynchronized;
    const char *ntpServer;

public:
    NTPManager(const char *server = "pool.ntp.org", long offset = 0, unsigned long interval = 60000);
    ~NTPManager();

    void begin();
    bool update();
    bool isTimeSet() const;
    String getFormattedTime() const;
    unsigned long getEpochTime() const;
    void setUpdateInterval(unsigned long interval);
    void setTimeOffset(long offset);
    NTPClient *getTimeClient();
};