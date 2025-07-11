#include "ph_sensor.h"

PH_Sensor::PH_Sensor(Preferences &preferences) : preferences(preferences)
{
    this->neutral_voltage = 0;
    this->acid_voltage = 0;
    this->ph_value = 0;
}

void PH_Sensor::begin()
{
    this->preferences.begin("ph_sensor", false);
    this->neutral_voltage = this->preferences.getFloat("neutral_voltage", 0);
    this->acid_voltage = this->preferences.getFloat("acid_voltage", 0);

    this->preferences.end();
}

void PH_Sensor::setNeutralVoltage(float voltage)
{
    this->preferences.begin("ph_sensor", false);
    this->preferences.putFloat("neutral_voltage", voltage);
    this->preferences.end();
    this->neutral_voltage = voltage;
}

void PH_Sensor::setAcidVoltage(float voltage)
{
    this->preferences.begin("ph_sensor", false);
    this->preferences.putFloat("acid_voltage", voltage);
    this->preferences.end();
    this->acid_voltage = voltage;
}

float PH_Sensor::getPHValue(float voltage)
{
    if (this->neutral_voltage == 0 || this->acid_voltage == 0)
    {
        return 0;
    }

    float slope = (7.0 - 4.0) / ((this->neutral_voltage - 1500.0) / 3.0 - (this->acid_voltage - 1500.0) / 3.0); // two point: (_neutralVoltage,7.0),(_acidVoltage,4.0)
    float intercept = 7.0 - slope * (this->neutral_voltage - 1500.0) / 3.0;
    // Serial.print("slope:");
    // Serial.print(slope);
    // Serial.print(",intercept:");
    // Serial.println(intercept);
    this->ph_value = slope * (voltage - 1500.0) / 3.0 + intercept; // y = k*x + b
    return ph_value;
}