#pragma once

#include <Arduino.h>
#include <Preferences.h>

class PH_Sensor
{
public:
    PH_Sensor(Preferences &preferences);
    void begin();

    float getPHValue(float voltage);

    void setNeutralVoltage(float voltage);
    void setAcidVoltage(float voltage);

private:
    Preferences &preferences;
    float neutral_voltage, acid_voltage;
    float ph_value;
};
