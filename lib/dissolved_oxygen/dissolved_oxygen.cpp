#include "dissolved_oxygen.h"

DissolvedOxygen::DissolvedOxygen(uint32_t pin) : pin(pin)
{
}

void DissolvedOxygen::begin(uint32_t callibrationVoltage, uint32_t callibrationTemperature)
{
    this->callibrationVoltage = callibrationVoltage;
    this->callibrationTemperature = callibrationTemperature;
}

float DissolvedOxygen::getDissolvedOxygen(uint8_t temperature)
{
    uint16_t ADC_Raw = analogRead(pin);
    uint16_t ADC_Voltage = uint32_t(VREF) * ADC_Raw / ADC_RES;

    // Serial.println("ADC_RAW: " + String(ADC_Raw) + "\tVoltage: " + String(ADC_Voltage));

    if (ADC_Raw == 0)
        return 0.0;

    uint16_t V_saturation = (uint32_t)callibrationVoltage + (uint32_t)35 * temperature - (uint32_t)callibrationTemperature * 35;
    return float((ADC_Voltage * DO_Table[temperature] / V_saturation) / 1000);
}