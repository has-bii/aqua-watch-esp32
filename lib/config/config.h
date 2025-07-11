#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <Preferences.h>

class Config
{
public:
    Config(Preferences &preferences);
    void begin();
    bool isConfigured();
    void setEmail(String email);
    void setPassword(String password);
    void setEnvId(String env_id);
    void setSSID(String ssid);
    void setPasswordWifi(String password_wifi);
    String getSSID();
    String getPasswordWifi();
    String getEmail();
    String getPassword();
    String getEnvId();
    void clearConfig();
    void printConfig();
    void clearWifiConfig();
    bool isWifiConfigured();
    bool isUserConfigured();
    void setActiveMenu(int menu);
    int getActiveMenu();
    bool isSynchronizationEnabled();
    void setSynchronizationEnabled(bool enabled);

private:
    Preferences &preferences;
    String email;
    String password;
    String env_id;
    String SSID;
    String password_wifi;
    int active_menu = 0;
    bool enabled_synchronization;
};

#endif