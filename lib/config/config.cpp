#include "config.h"

Config::Config(Preferences &prf) : preferences(prf)
{
    email = "";
    password = "";
    env_id = "";
    SSID = "";
    password_wifi = "";
}

void Config::begin()
{
    preferences.begin("config", false);

    email = preferences.getString("email", "");
    password = preferences.getString("password", "");
    env_id = preferences.getString("env_id", "");
    SSID = preferences.getString("SSID", "");
    password_wifi = preferences.getString("password_wifi", "");

    active_menu = preferences.getInt("active_menu", 0);
    enabled_synchronization = preferences.getBool("enabled_synchronization", false);

    preferences.end();
}

bool Config::isConfigured()
{
    return !email.isEmpty() && !password.isEmpty() && !env_id.isEmpty() && !SSID.isEmpty() && !password_wifi.isEmpty();
}

void Config::printConfig()
{
    Serial.println("Current Config:");
    Serial.printf("Email: %s\n", email.c_str());
    Serial.printf("Password: %s\n", password.c_str());
    Serial.printf("Env ID: %s\n", env_id.c_str());
    Serial.printf("SSID: %s\n", SSID.c_str());
    Serial.printf("Password Wifi: %s\n", password_wifi.c_str());

    Serial.println("");
    Serial.println("");
}

String Config::getEmail()
{
    return email;
}

String Config::getPassword()
{
    return password;
}

String Config::getEnvId()
{
    return env_id;
}

String Config::getSSID()
{
    return SSID;
}

String Config::getPasswordWifi()
{
    return password_wifi;
}

void Config::clearConfig()
{
    preferences.begin("config", false);

    preferences.clear();
    preferences.end();

    email = "";
    password = "";
    env_id = "";
    SSID = "";
    password_wifi = "";

    Serial.println("Config cleared");
}

void Config::setEmail(String newEmail)
{
    preferences.begin("config", false);

    email = newEmail;
    preferences.putString("email", email);

    preferences.end();
}
void Config::setPassword(String newPassword)
{
    preferences.begin("config", false);

    password = newPassword;
    preferences.putString("password", password);

    preferences.end();
}
void Config::setEnvId(String new_env_id)
{
    preferences.begin("config", false);

    env_id = new_env_id;
    preferences.putString("env_id", env_id);

    preferences.end();
}

void Config::setSSID(String newSSID)
{
    preferences.begin("config", false);

    SSID = newSSID;
    preferences.putString("SSID", SSID);

    preferences.end();
}

void Config::setPasswordWifi(String newPasswordWifi)
{
    preferences.begin("config", false);

    password_wifi = newPasswordWifi;
    preferences.putString("password_wifi", password_wifi);

    preferences.end();
}

void Config::clearWifiConfig()
{
    preferences.begin("config", false);

    SSID = "";
    password_wifi = "";
    preferences.putString("SSID", SSID);
    preferences.putString("password_wifi", password_wifi);

    preferences.end();
}

bool Config::isWifiConfigured()
{
    return !SSID.isEmpty() && !password_wifi.isEmpty();
}

bool Config::isUserConfigured()
{
    return !email.isEmpty() && !password.isEmpty();
}

void Config::setActiveMenu(int menu)
{
    preferences.begin("config", false);

    active_menu = menu;
    preferences.putInt("active_menu", active_menu);

    preferences.end();
}

int Config::getActiveMenu()
{
    return active_menu;
}

bool Config::isSynchronizationEnabled()
{
    return enabled_synchronization;
}

void Config::setSynchronizationEnabled(bool enabled)
{
    preferences.begin("config", false);

    enabled_synchronization = enabled;
    preferences.putBool("enabled_synchronization", enabled_synchronization);

    preferences.end();
}
