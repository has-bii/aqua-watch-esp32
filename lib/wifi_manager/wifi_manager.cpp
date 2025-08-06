#include <wifi_manager.h>

WiFiManager::WiFiManager(LCDManager &dsp) : lcd(dsp)
{
    // Constructor implementation
}

void WiFiManager::connect(const char *ssid, const char *password)
{
    lcd.print("Connecting...", "", 0);

    unsigned long startAttemptTime = millis();

    WiFi.begin(ssid, password);

    // Wait for connection with timeout
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < AQUAWATCH_WIFI_CONNECT_TIMEOUT)
    {
        delay(500);
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        lcd.print("WiFi connected", WiFi.localIP().toString(), 1000);
        delay(1000);
    }
    else
    {
        lcd.print("Failed to", "connect", 1000);
        delay(1000);
    }
}

void WiFiManager::connect(const char *ssid, const char *password, bool *isConnecting)
{
    if (isConnecting)
    {
        *isConnecting = true;
    }

    connect(ssid, password);

    if (isConnecting)
    {
        *isConnecting = false;
    }
}

bool WiFiManager::isConnected()
{
    return WiFi.status() == WL_CONNECTED;
}

void WiFiManager::begin()
{
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP("Aqua Watch");
}