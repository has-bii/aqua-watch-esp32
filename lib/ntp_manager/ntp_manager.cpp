#include "ntp_manager.h"

NTPManager::NTPManager(const char *server, long offset, unsigned long interval)
{
    ntpServer = server;
    timeOffset = offset;
    updateInterval = interval;
    lastUpdateAttempt = 0;
    ntpSynchronized = false;

    // Create NTP client
    timeClient = new NTPClient(ntpUDP, ntpServer, timeOffset, updateInterval);
}

NTPManager::~NTPManager()
{
    if (timeClient)
    {
        delete timeClient;
        timeClient = nullptr;
    }
}

void NTPManager::begin()
{
    if (timeClient)
    {
        timeClient->begin();
        Serial.println("NTP client initialized");
    }
}

bool NTPManager::update()
{
    if (!timeClient)
        return false;

    unsigned long currentMillis = millis();

    // Only attempt update if WiFi is connected and enough time has passed
    if (WiFi.status() == WL_CONNECTED &&
        (currentMillis - lastUpdateAttempt >= updateInterval || lastUpdateAttempt == 0))
    {

        lastUpdateAttempt = currentMillis;
        bool updated = timeClient->update();
        ntpSynchronized = timeClient->isTimeSet();

        if (updated && ntpSynchronized)
        {
            Serial.print("NTP time synchronized: ");
            Serial.println(timeClient->getFormattedTime());
        }
        else if (!updated)
        {
            Serial.println("NTP update failed");
        }

        return updated;
    }

    return false;
}

bool NTPManager::isTimeSet() const
{
    return timeClient && ntpSynchronized;
}

String NTPManager::getFormattedTime() const
{
    if (timeClient && ntpSynchronized)
    {
        return timeClient->getFormattedTime();
    }
    return "00:00:00";
}

unsigned long NTPManager::getEpochTime() const
{
    if (timeClient && ntpSynchronized)
    {
        return timeClient->getEpochTime();
    }
    return 0;
}

void NTPManager::setUpdateInterval(unsigned long interval)
{
    updateInterval = interval;
    if (timeClient)
    {
        timeClient->setUpdateInterval(interval);
    }
}

void NTPManager::setTimeOffset(long offset)
{
    timeOffset = offset;
    if (timeClient)
    {
        timeClient->setTimeOffset(offset);
    }
}

NTPClient *NTPManager::getTimeClient()
{
    return timeClient;
}