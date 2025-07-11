#pragma once

#include <Arduino.h>

#define VREF 5000
#define ADC_RES 4095

const uint16_t DO_Table[41] = {
    14460, 14220, 13820, 13440, 13090, 12740, 12420, 12110, 11810, 11530,
    11260, 11010, 10770, 10530, 10300, 10080, 9860, 9660, 9460, 9270,
    9080, 8900, 8730, 8570, 8410, 8250, 8110, 7960, 7820, 7690,
    7560, 7430, 7300, 7180, 7070, 6950, 6840, 6730, 6630, 6530, 6410};

class DissolvedOxygen
{
private:
    uint32_t callibrationVoltage;
    uint32_t callibrationTemperature;
    uint32_t pin;

public:
    DissolvedOxygen(uint32_t pin);
    void begin(uint32_t callibrationVoltage, uint32_t callibrationTemperature);
    float getDissolvedOxygen(uint8_t temperature);
};
