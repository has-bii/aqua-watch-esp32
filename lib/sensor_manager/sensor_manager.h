#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <EEPROM.h>
#include <ph_sensor.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Preferences.h>

// Add these to your header file
static constexpr float DO_TEMP_COEFFICIENT = 35.0f;
static constexpr float DO_VOLTAGE_SCALE = 1000.0f;
static constexpr float DO_MIN_TEMP = 0.0f;
static constexpr float DO_MAX_TEMP = 40.0f;

class SensorManager
{
public:
    SensorManager(uint8_t pH_pin, uint8_t DO_pin, uint8_t roomTemperaturePin, uint8_t waterTemperaturePin, uint8_t flowSensorPin, Preferences &prf);

    void begin();
    void readSensors();

    bool phCallibration(uint8_t mode);

    float pH_value, DO_value;                  // Store the float values for pH and DO
    float room_temperature, water_temperature; // Store the temperature value
    float flowRate;

    float ph_voltage;
    float do_voltage;

    bool doCalibration(float current_temperature);

private:
    Adafruit_ADS1115 ads;
    PH_Sensor ph;
    OneWire oneWireRoom;
    DallasTemperature roomTemperature;
    OneWire oneWireWater;
    DallasTemperature waterTemperature;
    Preferences &preferences;

    uint16_t doCallibrationVoltage;
    uint16_t doCallibrationTemperature;

    static volatile int flowPulseCount;

    float getDOValue();
    float getpHValue();
    void getTemperature();

    void doBegin();
    void flowRateBegin();

    uint8_t pH_pin;
    uint8_t DO_pin;
    uint8_t FLOW_SENSOR_PIN;

    float readDO();
    float readpH();
    float readFlowRate();
    static void pulseCounter();

    const uint16_t DO_Table[41] = {
        14460, 14220, 13820, 13440, 13090, 12740, 12420, 12110, 11810, 11530,
        11260, 11010, 10770, 10530, 10300, 10080, 9860, 9660, 9460, 9270,
        9080, 8900, 8730, 8570, 8410, 8250, 8110, 7960, 7820, 7690,
        7560, 7430, 7300, 7180, 7070, 6950, 6840, 6730, 6630, 6530, 6410};
};